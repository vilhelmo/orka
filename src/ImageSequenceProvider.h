/*
 * ImageSequenceProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef IMAGESEQUENCEPROVIDER_H_
#define IMAGESEQUENCEPROVIDER_H_

#include <string>
#include <vector>
#include <deque>

#include <QRunnable>
#include <QThreadPool>
#include <QTimer>
#include <QSemaphore>
#include <QMutex>
#include <QMutexLocker>

#include "ImageProvider.h"

namespace orka {

class ThreadedImageLoader: public QRunnable {
public:
    ThreadedImageLoader(OrkaImage * im) :
            image_(im) {
    }
    virtual ~ThreadedImageLoader() {
    }
    virtual void run();
private:
    OrkaImage * image_;
};

class ImageSequenceProvider: public ImageProvider {
Q_OBJECT
public:
    ImageSequenceProvider(const std::vector<std::string> & files);
    virtual ~ImageSequenceProvider();
    void cacheLoadNewClearOld();
    virtual std::pair<int, int> getFramerange();
    virtual std::string getColorSpace();
private slots:
    void start();
    void stop();
    void toggleStartStop();
    void jog(int dframes);
    void gotoFrame(int frame);

    void displayNextImage();
private:
    QTimer * display_timer_;
//    QThreadPool mImageLoaderThreadPool;
    int mCacheSizeNumImages;
    std::string first_image_color_space_;

    std::vector<std::string> mFiles;
    int mNumFiles;
    int mFileIndex;
    int mPrevFileIndex;
    int mLoadIndex;

    QSemaphore * mNumCachedSemaphore;
    std::vector<OrkaImage *> mImageCache;

    int mNumCachedFiles;
//    std::vector<ThreadedImageLoader *> load_image_threads_;
};

} /* namespace orka */
#endif /* IMAGESEQUENCEPROVIDER_H_ */
