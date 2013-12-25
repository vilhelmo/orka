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

OrkaImage::OrkaImage(std::string filename) :
        mPixels(NULL), mFilename(filename), mLoaded(false),
        mHeight(0), mWidth(0), mChannels(0){
    mLoadMutex = new QMutex();
}

OrkaImage::OrkaImage(int width, int height, int channels) :
        mLoaded(true), mWidth(width), mHeight(height), mChannels(channels) {
    // Used by VLCMovieProvider. Hard-coded to UCHAR format for now.
    mLoadMutex = new QMutex();
    format_ = OpenImageIO::TypeDesc::UCHAR;
    mPixels = (void *) malloc(
            mWidth * mHeight * mChannels * sizeof(format_.elementsize()));
}

OrkaImage::OrkaImage(const OrkaImage & other) {
    mLoadMutex = new QMutex();
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mChannels = other.mChannels;
    mFilename = other.mFilename;
    mLoaded = other.mLoaded;
    if (mLoaded) {
        mPixels = (void *) malloc(
                mWidth * mHeight * mChannels * other.format_.elementsize());
        memcpy(mPixels, other.mPixels,
                mWidth * mHeight * mChannels * other.format_.elementsize());
    }
}

OrkaImage::~OrkaImage() {
    QMutexLocker locker(mLoadMutex);
    if (mLoaded) {
        free(mPixels);
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
//    clock_t start = clock();
    OpenImageIO::ImageInput * open_image_ = OpenImageIO::ImageInput::open(
            mFilename);
    if (!open_image_) {
        throw OrkaException(
                std::string("Unable to open image: ") + mFilename + "\nError: "
                        + OpenImageIO::geterror());
    }
    const OpenImageIO::ImageSpec &spec = open_image_->spec();
    mWidth = spec.width;
    mHeight = spec.height;
    mChannels = spec.nchannels;
    format_ = spec.format;
    if (format_ == OpenImageIO::TypeDesc::HALF) {
        format_ = OpenImageIO::TypeDesc::FLOAT;
    }
    mPixels = (void *) malloc(
            mWidth * mHeight * mChannels * format_.elementsize());
    open_image_->read_image(format_, mPixels);
    open_image_->close();
    delete open_image_;
    mLoaded = true;
    locker.unlock();
    clock_t end = clock();
//    std::cout << "read image in " << ((float) end - start) / CLOCKS_PER_SEC
//            << " secs." << std::endl;
}

bool OrkaImage::isLoaded() {
    bool loaded;
    QMutexLocker locker(mLoadMutex);
    loaded = mLoaded;
    locker.unlock();
    return loaded;
}

void * OrkaImage::getPixels() {
    QMutexLocker locker(mLoadMutex);
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
        free(mPixels);
//		delete [] mPixels;
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
}

unsigned int OrkaImage::approxSize() {
    QMutexLocker locker(mLoadMutex);
    bool loaded = mLoaded;
    locker.unlock();
    if (loaded) {
        return mWidth * mHeight * mChannels * format_.elementsize(); //sizeof(float);
    } else {
        return 0;
    }
}

GLenum OrkaImage::glType() {
    switch (format_.basetype) {
    case OpenImageIO::TypeDesc::UCHAR:
        return GL_UNSIGNED_BYTE;
        break;
    case OpenImageIO::TypeDesc::CHAR:
        return GL_BYTE;
        break;
    case OpenImageIO::TypeDesc::USHORT:
        return GL_UNSIGNED_SHORT;
        break;
    case OpenImageIO::TypeDesc::SHORT:
        return GL_SHORT;
        break;
    case OpenImageIO::TypeDesc::UINT:
        return GL_UNSIGNED_INT;
        break;
    case OpenImageIO::TypeDesc::INT:
        return GL_INT;
        break;
    case OpenImageIO::TypeDesc::FLOAT:
        return GL_FLOAT;
        break;
    default:
        throw new OrkaException("Unknown oiio to gl format.");
        break;
    }
    return GL_FLOAT;
}

} /* namespace orka */
