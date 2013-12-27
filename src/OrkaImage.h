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

    float getPixel(int x, int y, int c) {
        QMutexLocker locker(mLoadMutex);
        bool loaded = mLoaded;
        locker.unlock();
        if (loaded) {
            switch (format_.basetype) {
            case OpenImageIO::TypeDesc::UCHAR:
                return (float) ((uchar *)mPixels)[(y * mWidth + x) * mChannels + c] / 255.0;
            case OpenImageIO::TypeDesc::CHAR:
                return (float) ((char *)mPixels)[(y * mWidth + x) * mChannels + c] / 255.0;
            case OpenImageIO::TypeDesc::USHORT:
                return (float) ((ushort *)mPixels)[(y * mWidth + x) * mChannels + c] / 255.0;
            case OpenImageIO::TypeDesc::SHORT:
                return (float) ((short *)mPixels)[(y * mWidth + x) * mChannels + c] / 255.0;
            case OpenImageIO::TypeDesc::UINT:
                return (float) ((uint *)mPixels)[(y * mWidth + x) * mChannels + c] / 255.0;
            case OpenImageIO::TypeDesc::INT:
                return (float) ((int *)mPixels)[(y * mWidth + x) * mChannels + c] / 255.0;
            case OpenImageIO::TypeDesc::FLOAT:
                return (float) ((float *)mPixels)[(y * mWidth + x) * mChannels + c];
            default:
                return NULL;
                break;
            }
    //        return (T) mPixels[(y * mWidth + x) * mChannels + c];
        } else {
            return NULL;
        }
    }

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
