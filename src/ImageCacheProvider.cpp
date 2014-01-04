/*
 * ImageSequenceProvider.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#include "ImageCacheProvider.h"
#include "OrkaImage.h"

namespace orka {

ImageCacheProvider::ImageCacheProvider(const std::vector<std::string> & files) :
    ImageProvider(), filenames_(files), num_files_(files.size()), file_index_(0) {
    // Load first image.
    OrkaImage * firstImage = new OrkaImage(filenames_.at(0));
    firstImage->loadImage();

    first_image_color_space_ = firstImage->color_space();

    image_cache_ = OpenImageIO::ImageCache::create();
    image_cache_->attribute("max_memory_MB", 500.0);
    image_cache_->attribute("max open files", 200);

    display_timer_ = new QTimer();
    QObject::connect(display_timer_, SIGNAL(timeout()), this,
            SLOT(displayNextImage()));
}

ImageCacheProvider::~ImageCacheProvider() {
    OpenImageIO::ImageCache::destroy(image_cache_);
}


std::pair<int, int> ImageCacheProvider::getFramerange() {
    return std::make_pair(1, num_files_);
}


std::string ImageCacheProvider::getColorSpace() {
    return first_image_color_space_;
}

void ImageCacheProvider::start() {
    if (num_files_ > 1) {
        display_timer_->start(41);
    } else {
        // Optimize case with a single image.
        display_timer_->singleShot(10, this, SLOT(displayNextImage()));
    }
}

void ImageCacheProvider::stop() {
    display_timer_->stop();
}

void ImageCacheProvider::toggleStartStop() {
    if (display_timer_->isActive()) {
        stop();
    } else {
        start();
    }
}

void ImageCacheProvider::jog(int dframes) {
    gotoFrame(file_index_ + dframes);
}

void ImageCacheProvider::gotoFrame(int frame) {
    file_index_ = (frame - 1) % num_files_;

    if (!display_timer_->isActive()) {
        display_timer_->singleShot(10, this, SLOT(displayNextImage()));
    }
}

void ImageCacheProvider::displayNextImage() {
    OrkaImage * image = new OrkaImage(image_cache_, filenames_[file_index_]);

    emit displayImage(image, file_index_+1, true);

//    delete image;

    file_index_ = (file_index_ + 1) % num_files_;
}

} /* namespace orka */
