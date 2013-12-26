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
    mImage->loadImage();
}

ImageSequenceProvider::ImageSequenceProvider(
        const std::vector<std::string> & files) :
        ImageProvider(), mFiles(files), mFileIndex(0), mPrevFileIndex(-1), mLoadIndex(
                0) {
    mNumFiles = mFiles.size();
    OrkaImage * firstImage = new OrkaImage(mFiles.at(0));
    mImageCache.push_back(firstImage);
    firstImage->loadImage();
    int approxSize = firstImage->approxSize();
    // TODO: Let the user configure cache size
    int cacheSize = 200 * 1000 * 1000; // 200 mb
    mCacheSizeNumImages = cacheSize / approxSize;
    for (int i = 1; i < mNumFiles; i += 1) {
        std::string filename = mFiles.at(i);
        OrkaImage * image = new OrkaImage(filename);
        mImageCache.push_back(image);
    }

    // Load first N images;
    for (OrkaImage * image : mImageCache) {
        ThreadedImageLoader * imageLoaderThread = new ThreadedImageLoader(
                image);
        mImageLoaderThreadPool.start(imageLoaderThread);
        mLoadIndex += 1;
        if (mLoadIndex > mCacheSizeNumImages) {
            break;
        }
    }
    mLoadIndex = mLoadIndex % mNumFiles;
    assert(mImageCache.size() == mFiles.size());

    display_timer_ = new QTimer();
    QObject::connect(display_timer_, SIGNAL(timeout()), this,
            SLOT(displayNextImage()));
}

ImageSequenceProvider::~ImageSequenceProvider() {
    for (OrkaImage * im : mImageCache) {
        delete im;
    }
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
    // TODO: delete old frames from memory
    mPrevFileIndex = mFileIndex;
    mFileIndex = std::max(std::min(mFileIndex + dframes, mNumFiles - 1), 0);
    mLoadIndex = mFileIndex;
    // Load the new image up.
    ThreadedImageLoader * imageLoaderThread = new ThreadedImageLoader(
            mImageCache.at(mLoadIndex));
    mImageLoaderThreadPool.start(imageLoaderThread);
    mLoadIndex = (mLoadIndex + 1) % mNumFiles;

    if (!display_timer_->isActive()) {
        display_timer_->singleShot(1, this, SLOT(displayNextImage()));
    }
}

void ImageSequenceProvider::displayNextImage() {
    if (mCacheSizeNumImages < mNumFiles &&
            mPrevFileIndex >= 0) {
        // Not all images fit in cache. Free one and load a new one up.
        //int prevIdx = (mNumFiles + mFileIndex - 1) % mNumFiles;
        // Free last image displayed.
        //mImageCache.at(prevIdx)->freePixels();
        mImageCache.at(mPrevFileIndex)->freePixels();

        // Load another image.
        ThreadedImageLoader * imageLoaderThread = new ThreadedImageLoader(
                mImageCache.at(mLoadIndex));
        mImageLoaderThreadPool.start(imageLoaderThread);
        mLoadIndex = (mLoadIndex + 1) % mNumFiles;
    }

    OrkaImage * image = mImageCache.at(mFileIndex);
    mPrevFileIndex = mFileIndex;
    mFileIndex = (mFileIndex + 1) % mNumFiles;

    emit displayImage(image);
}

} /* namespace orka */
