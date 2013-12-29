/*
 * ImageSequenceProvider.cpp
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#include "ImageSequenceProvider.h"
#include "OrkaImage.h"

#include <algorithm>

namespace orka {

void ThreadedImageLoader::run() {
    image_->loadImage();
}

ImageSequenceProvider::ImageSequenceProvider(
        const std::vector<std::string> & files) :
        ImageProvider(), mFiles(files), mFileIndex(0), mPrevFileIndex(-1), mLoadIndex(
                0) {
    mNumFiles = mFiles.size();

    // Load first image.
    OrkaImage * firstImage = new OrkaImage(mFiles.at(0));
    mImageCache.push_back(firstImage);
    firstImage->loadImage();

    // Approximate cache size
    int approxSize = firstImage->approxSize();
    // TODO: Let the user configure cache size
    int cacheSize = 200 * 1000 * 1000; // 200 mb
    mCacheSizeNumImages = cacheSize / approxSize;
    mNumCachedFiles = 0;
    mNumCachedSemaphore = new QSemaphore(mCacheSizeNumImages);

    // Create the rest of the images.
    for (int i = 1; i < mNumFiles; i += 1) {
        std::string filename = mFiles.at(i);
        OrkaImage * image = new OrkaImage(filename);
        mImageCache.push_back(image);
    }

    // Start loading some images into the cache.
    this->cacheLoadNewClearOld();

    display_timer_ = new QTimer();
    QObject::connect(display_timer_, SIGNAL(timeout()), this,
            SLOT(displayNextImage()));
}

ImageSequenceProvider::~ImageSequenceProvider() {
    for (OrkaImage * im : mImageCache) {
        delete im;
    }
}

void ImageSequenceProvider::cacheLoadNewClearOld() {
    if (mCacheSizeNumImages > mNumFiles) {
        return;
    }
    // Not all images fit in cache. Free one and load a new one up.
    // Free last image displayed.
    if (mPrevFileIndex >= 0) {
        OrkaImage * image = mImageCache.at(mPrevFileIndex);
        if (image->isLoaded()) {
            image->freePixels();
        }
    }

    if (mLoadIndex != (mFileIndex + mCacheSizeNumImages - 1) % mNumFiles) {
        // Recache.
        mLoadIndex = mFileIndex;
    }
    while (mLoadIndex != (mFileIndex + mCacheSizeNumImages - 1) % mNumFiles) {
        ThreadedImageLoader * image_loader_thread = new ThreadedImageLoader(
                mImageCache.at(mLoadIndex));
        QThreadPool::globalInstance()->start(image_loader_thread);
        mLoadIndex = (mLoadIndex + 1) % mNumFiles;
    }
}

std::pair<int, int> ImageSequenceProvider::getFramerange() {
    return std::make_pair(1, mNumFiles);
}

void ImageSequenceProvider::start() {
    if (mNumFiles > 1) {
        display_timer_->start(41);
    } else {
        // Optimize case with a single image.
        display_timer_->singleShot(10, this, SLOT(displayNextImage()));
    }
}

void ImageSequenceProvider::stop() {
    display_timer_->stop();
}

void ImageSequenceProvider::toggleStartStop() {
    if (display_timer_->isActive()) {
        stop();
    } else {
        start();
    }
}

void ImageSequenceProvider::jog(int dframes) {
    gotoFrame(mFileIndex + dframes);
}

void ImageSequenceProvider::gotoFrame(int frame) {
    mPrevFileIndex = mFileIndex;
    mFileIndex = (frame - 1) % mNumFiles;

    this->cacheLoadNewClearOld();

    if (!display_timer_->isActive()) {
        display_timer_->singleShot(10, this, SLOT(displayNextImage()));
    }
}

void ImageSequenceProvider::displayNextImage() {
    OrkaImage * image = mImageCache.at(mFileIndex);
    if (!image->isLoaded()) {
        image->loadImage();
//        return; // safeguard
    }

    emit displayImage(image, mFileIndex);

    this->cacheLoadNewClearOld();

    mPrevFileIndex = mFileIndex;
    mFileIndex = (mFileIndex + 1) % mNumFiles;
}

} /* namespace orka */
