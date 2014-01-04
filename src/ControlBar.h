/*
 * ControlBar.h
 *
 *  Created on: Dec 27, 2013
 *      Author: vilhelm
 */

#ifndef SRC_CONTROLBAR_H_
#define SRC_CONTROLBAR_H_

#include <QWidget>
#include <QPushButton>
#include <QSlider>

namespace orka {

class ImageProvider;
class OrkaImage;

class ControlBar: public QWidget {
    Q_OBJECT

 public:
    ControlBar(QWidget * parent = NULL);
    virtual ~ControlBar();

    virtual QSize sizeHint() const {
        return QSize(100, 50);
    }
    virtual QSize minimumSizeHint() const {
        return QSize(100, 50);
    }

 public slots:
    void sliderPressed();
    void sliderReleased();
    void set_image_provider(ImageProvider * provider);
    void displayImage(OrkaImage * image, int frame, bool freeOldImageData);
    void gotoFirstFrame();
    void gotoLastFrame();
    void frameChanged(int frame);

 private:
    QPushButton * first_frame_button_;
    QPushButton * stop_button_;
    QPushButton * start_button_;
    QPushButton * last_frame_button_;
    QSlider * frame_slider_;

    bool slider_moving_;

    ImageProvider * image_provider_;
};

} /* namespace orka */
#endif  // SRC_CONTROLBAR_H_
