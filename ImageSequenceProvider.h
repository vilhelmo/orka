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

#include "ImageProvider.h"

namespace orka {

class ThreadedImageLoader : public QRunnable {
public:
	ThreadedImageLoader(OrkaImage * im) : mImage(im) {}
	virtual ~ThreadedImageLoader() {}
	virtual void run();
private:
	OrkaImage * mImage;
};

class ImageSequenceProvider : public ImageProvider {
public:
	ImageSequenceProvider(const std::vector<std::string> & files);
	virtual ~ImageSequenceProvider();
	virtual ImageTimeStruct getImage();

private:
	QThreadPool mImageLoaderThreadPool;
//	QSemaphore * mCacheLimitSemaphore;
	int mCacheSizeNumImages;

	std::vector<std::string> mFiles;
	int mNumFiles;
	int mFileIndex;
	int mLoadIndex;
	std::vector<OrkaImage *> mImageCache;
};

} /* namespace orka */
#endif /* IMAGESEQUENCEPROVIDER_H_ */
