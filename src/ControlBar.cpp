/*
 * ControlBar.cpp
 *
 *  Created on: Dec 27, 2013
 *      Author: vilhelm
 */

#include "ControlBar.h"

#include <iostream>

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include "ImageProvider.h"

namespace orka {

ControlBar::ControlBar(QWidget * parent) :
        QWidget(parent), image_provider_(NULL) {
    QHBoxLayout * layout = new QHBoxLayout(this);
    this->setLayout(layout);

    first_frame_button_ = new QPushButton("|<");
    stop_button_ = new QPushButton("#");
    start_button_ = new QPushButton(">");
    last_frame_button_ = new QPushButton(">|");
//    first_frame_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    stop_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    start_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    last_frame_button_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    frame_slider_ = new QSlider(Qt::Horizontal);
//    frame_slider_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    layout->addWidget(first_frame_button_);
    layout->addWidget(stop_button_);
    layout->addWidget(start_button_);
    layout->addWidget(last_frame_button_);
    layout->addWidget(frame_slider_);

    frame_slider_->setMinimum(1);
    frame_slider_->setMaximum(100);
    frame_slider_->setSingleStep(1);
    frame_slider_->setPageStep(1);
    frame_slider_->setSliderPosition(1);

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

ControlBar::~ControlBar() {
    // TODO Auto-generated destructor stub
}

void ControlBar::setImageProvider(ImageProvider * provider) {
    image_provider_ = provider;
    std::pair<int, int> framerange = image_provider_->getFramerange();
    frame_slider_->setMinimum(framerange.first);
    frame_slider_->setMaximum(framerange.second);
    frame_slider_->setSliderPosition(framerange.first);

    QObject::connect(frame_slider_, SIGNAL(sliderMoved(int)), this, SLOT(frameChanged(int)));
    QObject::connect(stop_button_, SIGNAL(clicked(bool)), image_provider_, SLOT(stop()));
    QObject::connect(start_button_, SIGNAL(clicked(bool)), image_provider_, SLOT(start()));
    QObject::connect(first_frame_button_, SIGNAL(clicked(bool)), this, SLOT(gotoFirstFrame()));
    QObject::connect(last_frame_button_, SIGNAL(clicked(bool)), this, SLOT(gotoLastFrame()));
}

void ControlBar::displayImage(OrkaImage * image, int frame) {
    frame_slider_->setSliderPosition(frame);
}

void ControlBar::gotoFirstFrame() {
    image_provider_->gotoFrame(image_provider_->getFramerange().first);
}

void ControlBar::gotoLastFrame() {
    image_provider_->gotoFrame(image_provider_->getFramerange().second);
}

void ControlBar::frameChanged(int frame) {
    image_provider_->gotoFrame(frame);
}

} /* namespace orka */
