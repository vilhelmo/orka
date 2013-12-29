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
                return static_cast<float>(static_cast<uchar *>(mPixels)[(y
                        * mWidth + x) * mChannels + c]) / 255.0;
            case OpenImageIO::TypeDesc::CHAR:
                return static_cast<float>(static_cast<char *>(mPixels)[(y
                        * mWidth + x) * mChannels + c]) / 255.0;
            case OpenImageIO::TypeDesc::USHORT:
                return static_cast<float>(static_cast<ushort *>(mPixels)[(y
                        * mWidth + x) * mChannels + c]) / 255.0;
            case OpenImageIO::TypeDesc::SHORT:
                return static_cast<float>(static_cast<short *>(mPixels)[(y
                        * mWidth + x) * mChannels + c]) / 255.0;
            case OpenImageIO::TypeDesc::UINT:
                return static_cast<float>(static_cast<uint *>(mPixels)[(y
                        * mWidth + x) * mChannels + c]) / 255.0;
            case OpenImageIO::TypeDesc::INT:
                return static_cast<float>(static_cast<int *>(mPixels)[(y
                        * mWidth + x) * mChannels + c]) / 255.0;
            case OpenImageIO::TypeDesc::FLOAT:
                return static_cast<float *>(mPixels)[(y * mWidth + x)
                        * mChannels + c];
            default:
                return 0.f;
            }
        } else {
            return 0.f;
        }
    }

    float image_gamma() { return image_gamma_; }
    std::string color_space() { return color_space_; }

    // blind pointer to data.
    void * mPixels;
    std::string mFilename;
private:
    QMutex * mLoadMutex;
    OpenImageIO::TypeDesc format_;
    bool mLoaded;
    int mWidth;
    int mHeight;
    int mChannels;
    std::string color_space_;
    float image_gamma_;
};

} /* namespace orka */
#endif /* ORKAIMAGE_H_ */
