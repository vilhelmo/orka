/*
 * ImageSequenceProvider.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#include "VLCMovieProvider.h"

#include <string>
#include <utility>
#include <functional>
#include <algorithm>

#include "OrkaImage.h"

namespace orka {

static void vlc_display(void *data, void *id) {
    // VLC wants to display the video
    VLCMovieProvider * provider = reinterpret_cast<VLCMovieProvider *>(data);
    provider->display(id);
}

static void * vlc_lock(void *data, void **p_pixels) {
    // VLC prepares to render a video frame.
    VLCMovieProvider * provider = reinterpret_cast<VLCMovieProvider *>(data);
    provider->lock(p_pixels);
    return NULL;  // Picture identifier, not needed here.
}

static void vlc_unlock(void *data, void *id, void * const *p_pixels) {
    // VLC just rendered a video frame.
    VLCMovieProvider * provider = reinterpret_cast<VLCMovieProvider *>(data);
    provider->unlock(id, p_pixels);
}

static void vlc_media_duration_changed(const struct libvlc_event_t * event, void * data) {
    VLCMovieProvider * provider = reinterpret_cast<VLCMovieProvider *>(data);
    provider->setDuration(event->u.media_duration_changed.new_duration);
}

VLCMovieProvider::VLCMovieProvider(const std::string & filename) :
        ImageProvider(), filename_(filename) {
    char const *vlc_argv[] = {
            "--no-xlib",  // Don't use Xlib
            "--no-skip-frames",  //Disables framedropping on MPEG2 stream.
            "--text-renderer", "dummy",  // Text rendering module
            "--vout", "dummy",  // This is the the video output method used by VLC.
            "--no-video-title-show",  // Don't display the filename
            "--no-stats",  // Don't display stats
            "--no-sub-autodetect-file",  // Don't detect subtitles
            "--no-disable-screensaver",  // No need to disable the screensaver, and save a thread.
            };

    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

    // We launch VLC
    vlc_instance_ = libvlc_new(vlc_argc, vlc_argv);
    assert(vlc_instance_ != NULL);
    vlc_media_ = libvlc_media_new_path(vlc_instance_, filename_.c_str());
    vlc_media_player_ = libvlc_media_player_new_from_media(vlc_media_);
    libvlc_media_release(vlc_media_);

    max_time_ms_ = -1;

    libvlc_event_manager_t * vlc_event_manager = libvlc_media_event_manager(vlc_media_);
    libvlc_event_attach(vlc_event_manager, libvlc_MediaDurationChanged, vlc_media_duration_changed, this);
    // Start and stop the video to trigger the MediaDurationChanged event, which sets max_time_ms_.
    libvlc_media_player_play(vlc_media_player_);
    libvlc_media_player_stop(vlc_media_player_);

    libvlc_video_get_size(vlc_media_player_, 0, &width_, &height_);
    assert(width_ > 0 && height_ > 0);

    channels_ = 3;
    // TODO: get fps, but according to the internet, not all media has fps
    // information available. Google 'libvlc_media_player_get_fps' for more info.
    fps_ = 24.0;

    // Allocate space for pixel data.
    frameData_ = new vlc_int[width_ * height_];
    currentFrame_ = new OrkaImage(width_, height_, channels_);

    // Setup the video callbacks -
    // these are the ones that actually transfers and displays the images
    libvlc_video_set_callbacks(vlc_media_player_, vlc_lock, vlc_unlock,
            vlc_display, this);
    libvlc_video_set_format(vlc_media_player_, "RV32", width_, height_,
            width_ * sizeof(vlc_int));
    /*  These formats work, there are some more YUV based ones I haven't tried.
     *  RGBA: ? 32bppRgba assumed.
     *    RV16: PixelFormat.Format16bppRgb565;
     *    RV24: PixelFormat.Format24bppRgb;
     *    RV32:PixelFormat.Format32bppRgb;
     */
}

VLCMovieProvider::~VLCMovieProvider() {
    libvlc_media_player_release(vlc_media_player_);
    libvlc_release(vlc_instance_);
    delete [] frameData_;
    delete currentFrame_;
}

std::pair<int, int> VLCMovieProvider::getFramerange() {
    return std::make_pair(1, fps_ * static_cast<float>(max_time_ms_) / 1000.f);
}

void VLCMovieProvider::start() {
    libvlc_media_player_play(vlc_media_player_);
}

void VLCMovieProvider::stop() {
    libvlc_media_player_pause(vlc_media_player_);
}

void VLCMovieProvider::toggleStartStop() {
    libvlc_media_player_pause(vlc_media_player_);
}

void VLCMovieProvider::jog(int dframes) {
    libvlc_time_t current_time_ms = libvlc_media_player_get_time(
            vlc_media_player_);
    current_time_ms = std::max(
            std::min(current_time_ms + 10 * dframes, max_time_ms_),
            (libvlc_time_t) 0);
    libvlc_media_player_set_time(vlc_media_player_, current_time_ms);
}

void VLCMovieProvider::gotoFrame(int frame) {
    libvlc_time_t goto_ms = 1000 * (static_cast<float>(frame) / fps_);
    libvlc_media_player_set_time(vlc_media_player_, goto_ms);
}

void VLCMovieProvider::display(void *id) {
    for (uint i = 0; i < height_ * width_; i += 1) {
        vlc_int data = frameData_[i];
        int mask = (1 << 8) - 1;
        int b = (data >> 0) & mask;
        int g = (data >> 8) & mask;
        int r = (data >> 16) & mask;
//        int a = (data >> 24) & mask;  // No alpha support in videos right now.
        reinterpret_cast<uchar*>(currentFrame_->pixel_data_)[i * channels_ + 0] = r;
        reinterpret_cast<uchar*>(currentFrame_->pixel_data_)[i * channels_ + 1] = g;
        reinterpret_cast<uchar*>(currentFrame_->pixel_data_)[i * channels_ + 2] = b;
    }
    libvlc_time_t current_time_ms = libvlc_media_player_get_time(
            vlc_media_player_);
    int frame = fps_ * static_cast<float>(current_time_ms) / 1000.f;

    emit displayImage(currentFrame_, frame, false);
}

void VLCMovieProvider::lock(void ** p_pixels) {
    *p_pixels = frameData_;
}

void VLCMovieProvider::unlock(void *id, void * const *p_pixels) {
    assert(id == NULL); /* picture identifier, not needed here */
}

void VLCMovieProvider::setDuration(int new_duration) {
    max_time_ms_ = new_duration;
}

} /* namespace orka */
