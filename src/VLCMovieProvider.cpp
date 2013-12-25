/*
 * ImageSequenceProvider.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#include "VLCMovieProvider.h"

#include <functional>

#include "OrkaImage.h"

namespace orka {

static void vlc_display(void *data, void *id) {
	/* VLC wants to display the video */
	VLCMovieProvider * provider = (VLCMovieProvider *) data;
	provider->display(id);
}

// VLC prepares to render a video frame.
static void * vlc_lock(void *data, void **p_pixels) {
	VLCMovieProvider * provider = (VLCMovieProvider *) data;
	provider->lock(p_pixels);
	return NULL; // Picture identifier, not needed here.
}

// VLC just rendered a video frame.
static void vlc_unlock(void *data, void *id, void * const *p_pixels) {
	VLCMovieProvider * provider = (VLCMovieProvider *) data;
	provider->unlock(id, p_pixels);
}

VLCMovieProvider::VLCMovieProvider(const std::string & filename) :
		ImageProvider(), filename_(filename) {
	char const *vlc_argv[] = { "--no-audio", /* skip any audio track */
	"--no-xlib", /* tell VLC to not use Xlib */
			"--verbose=2", // Be much more verbose then normal for debugging purpose
			"--extraintf=logger", // Log anything
			"--ignore-config", // Don't use VLC's config
			"-v", "-I", "dummy", "--no-media-library", "--vout=dummy",
			"--aout=dummy" };
	int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

	// We launch VLC
	vlc_instance_ = libvlc_new(vlc_argc, vlc_argv);
	assert(vlc_instance_ != NULL);
	vlc_media_ = libvlc_media_new_path(vlc_instance_, filename_.c_str());
	vlc_media_player_ = libvlc_media_player_new_from_media(vlc_media_);
	libvlc_media_release(vlc_media_);

	//====
	// Start the video stream for a little bit to get the format, then stop it.
	// https://forum.videolan.org/viewtopic.php?f=32&t=84581
	libvlc_media_player_play(vlc_media_player_);
	unsigned int width, height = 0;
	while (width <= 0 && height <= 0) {
		libvlc_video_get_size(vlc_media_player_, 0, &width, &height);
	}
	libvlc_media_player_stop(vlc_media_player_);
	//====

	width_ = width;
	height_ = height;
	channels_ = 3;

	frameData_ = (vlc_int *) malloc(width_ * height_ * sizeof(vlc_int));
	currentFrame_ = new OrkaImage(width_, height_, channels_);
	libvlc_video_set_callbacks(vlc_media_player_, vlc_lock, vlc_unlock,
			vlc_display, this);
	libvlc_video_set_format(vlc_media_player_, "RGBA", width_, height_,
			width_ * sizeof(vlc_int));
	/*    RGBA: ? 32bpprgba assumed.
	 *    RV16: PixelFormat.Format16bppRgb565;
	 *    RV24: PixelFormat.Format24bppRgb;
	 *    RV32:PixelFormat.Format32bppRgb;
	 */
}

VLCMovieProvider::~VLCMovieProvider() {
	libvlc_media_player_release(vlc_media_player_);
	libvlc_release(vlc_instance_);
	free(frameData_);
	delete currentFrame_;
}

void VLCMovieProvider::start() {
	libvlc_media_player_play(vlc_media_player_);
}

void VLCMovieProvider::stop() {
	libvlc_media_player_stop(vlc_media_player_);
}

void VLCMovieProvider::toggleStartStop() {
	libvlc_media_player_pause(vlc_media_player_);
}

void VLCMovieProvider::display(void *id) {
	for (uint i = 0; i < height_ * width_; i += 1) {
		vlc_int data = frameData_[i];
		int mask = (1 << 8) - 1;
		int r = (data >> 0) & mask;
		int g = (data >> 8) & mask;
		int b = (data >> 16) & mask;
		int a = (data >> 24) & mask;
		currentFrame_->mPixels[i * channels_] = r / 256.0;
		currentFrame_->mPixels[i * channels_ + 1] = g / 256.0;
		currentFrame_->mPixels[i * channels_ + 2] = b / 256.0;
	}
	emit displayImage(currentFrame_);
}

void VLCMovieProvider::lock(void ** p_pixels) {
	*p_pixels = frameData_;
}

void VLCMovieProvider::unlock(void *id, void * const *p_pixels) {
	assert(id == NULL); /* picture identifier, not needed here */
}

} /* namespace orka */
