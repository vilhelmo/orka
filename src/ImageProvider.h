/*
 * ImageProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef SRC_IMAGEPROVIDER_H_
#define SRC_IMAGEPROVIDER_H_

#include <QObject>

#include <utility>
#include <string>

namespace orka {

class OrkaImage;


class ImageProvider: public QObject {
Q_OBJECT

 public:
    ImageProvider();
    virtual ~ImageProvider();
    virtual std::pair<int, int> getFramerange() = 0;
    virtual std::string getColorSpace() = 0;

 public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void toggleStartStop() = 0;
    virtual void jog(int dframes) = 0;
    virtual void gotoFrame(int frame) = 0;

 signals:
    void displayImage(OrkaImage * image, int frame, bool freeOldImageData);
};

}  // end namespace orka

#endif  // SRC_IMAGEPROVIDER_H_
