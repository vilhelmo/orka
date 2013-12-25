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
#include <GL/gl.h>

namespace orka {

class OrkaImage {
public:
    OrkaImage(std::string filename);
    OrkaImage(int width, int height, int channels);
    OrkaImage(const OrkaImage & other);
    virtual ~OrkaImage();
    void loadImage();
    bool isLoaded();
    void * getPixels();
    void freePixels();
    unsigned int width();
    unsigned int height();
    unsigned int channels();
    unsigned int approxSize();
    GLenum glType();

    // blind pointer to data.
    void * mPixels;
private:
    QMutex * mLoadMutex;
    std::string mFilename;
    OpenImageIO::TypeDesc format_;
    bool mLoaded;
    int mWidth;
    int mHeight;
    int mChannels;

};

} /* namespace orka */
#endif /* ORKAIMAGE_H_ */
