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
        mPixels(NULL), mFilename(filename), loaded_(false),
        height_(0), width_(0), channels_(0){
    load_mutex_ = new QMutex();
}

OrkaImage::OrkaImage(int width, int height, int channels) :
        loaded_(true), width_(width), height_(height), channels_(channels) {
    // Used by VLCMovieProvider. Hard-coded to UCHAR format for now.
    load_mutex_ = new QMutex();
    format_ = OpenImageIO::TypeDesc::UCHAR;
    mPixels = (void *) malloc(
            width_ * height_ * channels_ * sizeof(format_.elementsize()));
}

OrkaImage::OrkaImage(const OrkaImage & other) {
    load_mutex_ = new QMutex();
    width_ = other.width_;
    height_ = other.height_;
    channels_ = other.channels_;
    mFilename = other.mFilename;
    loaded_ = other.loaded_;
    if (loaded_) {
        mPixels = (void *) malloc(
                width_ * height_ * channels_ * other.format_.elementsize());
        memcpy(mPixels, other.mPixels,
                width_ * height_ * channels_ * other.format_.elementsize());
    }
}

OrkaImage::~OrkaImage() {
    QMutexLocker locker(load_mutex_);
    if (loaded_) {
        free(mPixels);
    }
    locker.unlock();
}

void OrkaImage::loadImage() {
    QMutexLocker locker(load_mutex_);
    if (loaded_) {
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
    width_ = spec.width;
    height_ = spec.height;
    channels_ = spec.nchannels;
    format_ = spec.format;
    if (format_ == OpenImageIO::TypeDesc::HALF) {
        format_ = OpenImageIO::TypeDesc::FLOAT;
    }
    color_space_ = spec.get_string_attribute("oiio:ColorSpace", "GammaCorrected");
    if (color_space_ == "GammaCorrected") {
        image_gamma_ = spec.get_float_attribute("oiio:Gamma", 2.2);
    } else {
        image_gamma_ = 1.0;
    }

    mPixels = (void *) malloc(
            width_ * height_ * channels_ * format_.elementsize());
    open_image_->read_image(format_, mPixels);
    open_image_->close();
    delete open_image_;

    loaded_ = true;
    locker.unlock();
//    clock_t end = clock();
//    std::cout << "read image in " << ((float) end - start) / CLOCKS_PER_SEC
//            << " secs." << std::endl;

}

bool OrkaImage::isLoaded() {
    bool loaded;
    QMutexLocker locker(load_mutex_);
    loaded = loaded_;
    locker.unlock();
    return loaded;
}

void * OrkaImage::getPixels() {
    QMutexLocker locker(load_mutex_);
    bool loaded = loaded_;
    locker.unlock();
    if (loaded) {
        return mPixels;
    } else {
        return NULL;
    }
}

void OrkaImage::freePixels() {
    QMutexLocker locker(load_mutex_);
    if (loaded_) {
        free(mPixels);
        loaded_ = false;
    }
    locker.unlock();
}

unsigned int OrkaImage::width() {
    QMutexLocker locker(load_mutex_);
    bool loaded = loaded_;
    locker.unlock();
    if (loaded) {
        return width_;
    } else {
        return 0;
    }
}

unsigned int OrkaImage::height() {
    QMutexLocker locker(load_mutex_);
    bool loaded = loaded_;
    locker.unlock();
    if (loaded) {
        return height_;
    } else {
        return 0;
    }
}

unsigned int OrkaImage::channels() {
    QMutexLocker locker(load_mutex_);
    bool loaded = loaded_;
    locker.unlock();
    if (loaded) {
        return channels_;
    } else {
        return 0;
    }
}

unsigned int OrkaImage::approxSize() {
    QMutexLocker locker(load_mutex_);
    bool loaded = loaded_;
    locker.unlock();
    if (loaded) {
        return width_ * height_ * channels_ * format_.elementsize();
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
