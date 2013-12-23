/*
 * OrkaImage.h
 *
 *  Created on: Dec 15, 2013
 *      Author: vilhelm
 */

#ifndef ORKAIMAGE_H_
#define ORKAIMAGE_H_

#include <string>
#include <QMutex>
#include <OpenImageIO/imageio.h>

namespace orka {

class OrkaImage {
public:
	OrkaImage(std::string filename);
	virtual ~OrkaImage();
	void loadImage();
	bool isLoaded();
	float * getPixels(bool waitForLoad=true);
	void freePixels();
	unsigned int width();
	unsigned int height();
	unsigned int channels();
	unsigned int approxSize();
private:
	QMutex * mLoadMutex;
	std::string mFilename;
	bool mLoaded;
	float * mPixels;
	int mWidth;
	int mHeight;
	int mChannels;
};

} /* namespace orka */
#endif /* ORKAIMAGE_H_ */
