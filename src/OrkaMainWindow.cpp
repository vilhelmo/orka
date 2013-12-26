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

namespace orka {

OrkaMainWindow::OrkaMainWindow(OrkaViewSettings * view_settings)
{
	mGLImageDisplayWidget = new GLImageDisplayWidget(view_settings);

    this->setCentralWidget(mGLImageDisplayWidget);

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
    QAction * reset_zoom = new QAction("Reset zoom 100%", controlMenu);
    QAction * fit_zoom = new QAction("Fit zoom to window", controlMenu);
//    reset_zoom->setShortcut(QKeySequence("-"));

    fileMenu->addAction(exit);
    controlMenu->addAction(togglePlayPause);
    controlMenu->addAction(increase_exposure);
    controlMenu->addAction(decrease_exposure);
    controlMenu->addAction(reset_zoom);
    controlMenu->addAction(fit_zoom);
    helpMenu->addAction(aboutQt);

    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(aboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    QObject::connect(togglePlayPause, SIGNAL(triggered(bool)), mGLImageDisplayWidget, SLOT(togglePlayPause()));
    QObject::connect(increase_exposure, SIGNAL(triggered(bool)), view_settings, SLOT(increaseExposure()));
    QObject::connect(decrease_exposure, SIGNAL(triggered(bool)), view_settings, SLOT(decreaseExposure()));
    QObject::connect(reset_zoom, SIGNAL(triggered(bool)), view_settings, SLOT(resetZoom()));
    QObject::connect(fit_zoom, SIGNAL(triggered(bool)), mGLImageDisplayWidget, SLOT(fitZoomToWindow()));
}

void OrkaMainWindow::setImageProvider(ImageProvider * provider) {
	mGLImageDisplayWidget->setImageProvider(provider);
	QObject::connect(provider, SIGNAL(displayImage(OrkaImage *)), mGLImageDisplayWidget, SLOT(displayImage(OrkaImage *)));
    mGLImageDisplayWidget->start();
}

} // end namespace orka

