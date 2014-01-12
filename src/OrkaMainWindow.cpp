#include "OrkaMainWindow.h"
#include "GLImageDisplayWidget.h"

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QTimer>

#include "OrkaViewSettings.h"
#include "ImageProvider.h"
#include "ControlBar.h"

namespace orka {

OrkaMainWindow::OrkaMainWindow(OrkaViewSettings * view_settings) {
    image_display_gl_widget_ = new GLImageDisplayWidget(view_settings);

    central_widget_ = new QWidget(this);
    QVBoxLayout * central_layout = new QVBoxLayout(central_widget_);
    central_widget_->setLayout(central_layout);
    central_layout->addWidget(image_display_gl_widget_);
    control_bar_ = new ControlBar(this);
    central_layout->addWidget(control_bar_);
    this->setCentralWidget(central_widget_);

    QMenu *fileMenu = new QMenu("File");
    QMenu *controlMenu = new QMenu("Control");
    QMenu *helpMenu = new QMenu("Help");
    this->menuBar()->addMenu(fileMenu);
    this->menuBar()->addMenu(controlMenu);
    this->menuBar()->addMenu(helpMenu);

    QAction *exit = new QAction("&Quit", fileMenu);
    exit->setShortcuts(QKeySequence::Quit);
    QAction *aboutQt = new QAction("AboutQt", helpMenu);

    QAction * togglePlayPause = new QAction("Play/Pause", controlMenu);
    togglePlayPause->setShortcut(QKeySequence(" "));

    QAction * increase_exposure = new QAction("Increase Exposure", controlMenu);
    increase_exposure->setShortcut(QKeySequence("Shift++"));
    QAction * decrease_exposure = new QAction("Decrease Exposure", controlMenu);
    decrease_exposure->setShortcut(QKeySequence("-"));
    QAction * fit_zoom = new QAction("Fit zoom to window", controlMenu);
    fit_zoom->setShortcut(QKeySequence("0"));
    QAction * reset_zoom = new QAction("Set zoom 100%", controlMenu);
    reset_zoom->setShortcut(QKeySequence("1"));
    QAction * fullscreen = new QAction("Show Fullscreen", controlMenu);
    fullscreen->setShortcut(QKeySequence("F11"));

    fileMenu->addAction(exit);
    controlMenu->addAction(togglePlayPause);
    controlMenu->addAction(increase_exposure);
    controlMenu->addAction(decrease_exposure);
    controlMenu->addAction(reset_zoom);
    controlMenu->addAction(fit_zoom);
    controlMenu->addAction(fullscreen);
    helpMenu->addAction(aboutQt);

    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(aboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    QObject::connect(togglePlayPause, SIGNAL(triggered(bool)),
            image_display_gl_widget_, SLOT(togglePlayPause()));
    QObject::connect(increase_exposure, SIGNAL(triggered(bool)), view_settings,
    SLOT(increaseExposure()));
    QObject::connect(decrease_exposure, SIGNAL(triggered(bool)), view_settings,
    SLOT(decreaseExposure()));
    QObject::connect(reset_zoom, SIGNAL(triggered(bool)), view_settings,
    SLOT(resetZoom()));
    QObject::connect(fit_zoom, SIGNAL(triggered(bool)),
            image_display_gl_widget_, SLOT(fitZoomToWindow()));
    QObject::connect(fullscreen, SIGNAL(triggered(bool)), this,
            SLOT(toggleFullscreen()));
}

void OrkaMainWindow::set_image_provider(ImageProvider * provider) {
    image_display_gl_widget_->set_image_provider(provider);
    QObject::connect(provider, SIGNAL(displayImage(OrkaImage *, int, bool)),
            image_display_gl_widget_,
            SLOT(displayImage(OrkaImage *, int, bool)));

    control_bar_->set_image_provider(provider);
    QObject::connect(provider, SIGNAL(displayImage(OrkaImage *, int, bool)),
            control_bar_, SLOT(displayImage(OrkaImage *, int, bool)));

    image_display_gl_widget_->start();
}

void OrkaMainWindow::toggleFullscreen() {
    if (this->isFullScreen()) {
        this->showNormal();
    } else {
        this->showFullScreen();
    }
}

} // end namespace orka

