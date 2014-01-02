/*
 * OrkaImage.h
 *
 *  Created on: Dec 15, 2013
 *      Author: vilhelm
 */

#ifndef SRC_ORKAIMAGE_H_
#define SRC_ORKAIMAGE_H_

#include <QMutex>
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagecache.h>
#include <GL/gl.h>

#include <string>

namespace orka {

class OrkaImage {
 public:
    OrkaImage(std::string filename);
    OrkaImage(OpenImageIO::ImageCache * cache, std::string filename);
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
        QMutexLocker locker(load_mutex_);
        bool loaded = loaded_;
        locker.unlock();
        if (loaded) {
            switch (format_.basetype) {
            case OpenImageIO::TypeDesc::UCHAR:
                return static_cast<float>(static_cast<uchar *>(pixel_data_)[(y
                        * width_ + x) * channels_ + c]) / 255.0;
            case OpenImageIO::TypeDesc::CHAR:
                return static_cast<float>(static_cast<char *>(pixel_data_)[(y
                        * width_ + x) * channels_ + c]) / 255.0;
            case OpenImageIO::TypeDesc::USHORT:
                return static_cast<float>(static_cast<ushort *>(pixel_data_)[(y
                        * width_ + x) * channels_ + c]) / 255.0;
            case OpenImageIO::TypeDesc::SHORT:
                return static_cast<float>(static_cast<short *>(pixel_data_)[(y
                        * width_ + x) * channels_ + c]) / 255.0;
            case OpenImageIO::TypeDesc::UINT:
                return static_cast<float>(static_cast<uint *>(pixel_data_)[(y
                        * width_ + x) * channels_ + c]) / 255.0;
            case OpenImageIO::TypeDesc::INT:
                return static_cast<float>(static_cast<int *>(pixel_data_)[(y
                        * width_ + x) * channels_ + c]) / 255.0;
            case OpenImageIO::TypeDesc::FLOAT:
                return static_cast<float *>(pixel_data_)[(y * width_ + x)
                        * channels_ + c];
            default:
                return 0.f;
            }
        } else {
            return 0.f;
        }
    }

    float image_gamma() {
        return image_gamma_;
    }
    std::string color_space() {
        return color_space_;
    }

    // blind pointer to data.
    void * pixel_data_;
    std::string filename_;

 private:
    QMutex * load_mutex_;
    OpenImageIO::TypeDesc format_;
    bool loaded_;
    int width_;
    int height_;
    int channels_;
    std::string color_space_;
    float image_gamma_;
};

} /* namespace orka */

#endif  // SRC_ORKAIMAGE_H_
