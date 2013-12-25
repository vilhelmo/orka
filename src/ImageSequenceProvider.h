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

#include "ImageProvider.h"

namespace orka {

class ThreadedImageLoader: public QRunnable {
public:
	ThreadedImageLoader(OrkaImage * im) :
			mImage(im) {
	}
	virtual ~ThreadedImageLoader() {
	}
	virtual void run();
private:
	OrkaImage * mImage;
};

class ImageSequenceProvider: public ImageProvider {
Q_OBJECT
public:
	ImageSequenceProvider(const std::vector<std::string> & files);
	virtual ~ImageSequenceProvider();
private slots:
	void start();
	void stop();
	void toggleStartStop();

	void displayNextImage();
private:
	QTimer * display_timer_;

	QThreadPool mImageLoaderThreadPool;
	int mCacheSizeNumImages;

	std::vector<std::string> mFiles;
	int mNumFiles;
	int mFileIndex;
	int mLoadIndex;
	std::vector<OrkaImage *> mImageCache;
};

} /* namespace orka */
#endif /* IMAGESEQUENCEPROVIDER_H_ */
