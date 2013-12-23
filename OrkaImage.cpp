/*
 * OrkaImage.cpp
 *
 *  Created on: Dec 15, 2013
 *      Author: vilhelm
 */

#include "OrkaImage.h"
#include "OrkaException.h"

#include <thread>
#include <chrono>

#include <QMutexLocker>

namespace orka {

OrkaImage::OrkaImage(std::string filename) : mFilename(filename), mLoaded(false) {
	mLoadMutex = new QMutex();
	//	OpenImageIO::ImageInput * image = OpenImageIO::ImageInput::open(filename);
//	if (!image) {
//		throw OrkaException(std::string("Unable to open image: ") + filename + "\nError: " + OpenImageIO::geterror());
//	}
//	const OpenImageIO::ImageSpec &spec = image->spec();
//	int xres = spec.width;
//	int yres = spec.height;
//	int channels = spec.nchannels;
//	mPixels = new float[xres * yres * channels];
//	mWidth = xres;
//	mHeight = yres;
//	mChannels = channels;
//	image->read_image(OpenImageIO::TypeDesc::FLOAT, &mPixels[0]);
//	image->close();
//	delete image;
}

OrkaImage::~OrkaImage() {
	QMutexLocker locker(mLoadMutex);
	if (mLoaded) {
		delete [] mPixels;
	}
	locker.unlock();
}

void OrkaImage::loadImage() {
	QMutexLocker locker(mLoadMutex);
	if (mLoaded) {
		// Image already loaded.
		locker.unlock();
		return;
	}
	OpenImageIO::ImageInput * image = OpenImageIO::ImageInput::open(mFilename);
	if (!image) {
		throw OrkaException(std::string("Unable to open image: ") + mFilename + "\nError: " + OpenImageIO::geterror());
	}
	const OpenImageIO::ImageSpec &spec = image->spec();
	int xres = spec.width;
	int yres = spec.height;
	int channels = spec.nchannels;
	mPixels = new float[xres * yres * channels];
	mWidth = xres;
	mHeight = yres;
	mChannels = channels;
	image->read_image(OpenImageIO::TypeDesc::FLOAT, &mPixels[0]);
	image->close();
	delete image;
	mLoaded = true;
	locker.unlock();
}

bool OrkaImage::isLoaded() {
	bool loaded;
	QMutexLocker locker(mLoadMutex);
	loaded = mLoaded;
	locker.unlock();
	return loaded;
}

float * OrkaImage::getPixels(bool waitForLoad) {
	QMutexLocker locker(mLoadMutex);
	if (waitForLoad) {
		locker.unlock();
		while (!mLoaded) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		locker.relock();
	}
	bool loaded = mLoaded;
	locker.unlock();
	if (loaded) {
		return mPixels;
	} else {
		return NULL;
	}
}

void OrkaImage::freePixels() {
	QMutexLocker locker(mLoadMutex);
	if (mLoaded) {
		delete [] mPixels;
		mLoaded = false;
	}
	locker.unlock();
}
unsigned int OrkaImage::width() {
	QMutexLocker locker(mLoadMutex);
	bool loaded = mLoaded;
	locker.unlock();
	if (loaded) {
		return mWidth;
	} else {
		return 0;
	}
}

unsigned int OrkaImage::height() {
	QMutexLocker locker(mLoadMutex);
	bool loaded = mLoaded;
	locker.unlock();
	if (loaded) {
		return mHeight;
	} else {
		return 0;
	}
}

unsigned int OrkaImage::channels() {
	QMutexLocker locker(mLoadMutex);
	bool loaded = mLoaded;
	locker.unlock();
	if (loaded) {
		return mChannels;
	} else {
		return 0;
	}
	locker.unlock();
}

unsigned int OrkaImage::approxSize() {
	QMutexLocker locker(mLoadMutex);
	bool loaded = mLoaded;
	locker.unlock();
	if (loaded) {
		return mWidth * mHeight * mChannels * sizeof(float);
	} else {
		return 0;
	}
}

} /* namespace orka */
