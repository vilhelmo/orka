/*
 * ImageCacheProvider.h
 *
 *  Created on: Jan 1, 2014
 *      Author: vilhelm
 */

#ifndef SRC_IMAGECACHEPROVIDER_H_
#define SRC_IMAGECACHEPROVIDER_H_

#include "ImageProvider.h"

#include <QObject>

#include <utility>
#include <string>

#include <QTimer>

#include <OpenImageIO/imagecache.h>

namespace orka {

class OrkaImage;

class ImageCacheProvider: public ImageProvider {
Q_OBJECT

 public:
    ImageCacheProvider(const std::vector<std::string> & files);
    virtual ~ImageCacheProvider();
    std::pair<int, int> getFramerange();
    std::string getColorSpace();

 public slots:
    void start();
    void stop();
    void toggleStartStop();
    void jog(int dframes);
    void gotoFrame(int frame);

    void displayNextImage();
 private:
    QTimer * display_timer_;
    std::string first_image_color_space_;

    OpenImageIO::ImageCache * image_cache_;

    std::vector<std::string> filenames_;
    int num_files_;
    int file_index_;
};

}  // end namespace orka

#endif  // SRC_IMAGECACHEPROVIDER_H_
