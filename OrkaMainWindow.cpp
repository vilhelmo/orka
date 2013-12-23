#include "OrkaMainWindow.h"
#include "GLImageDisplayWidget.h"

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QTimer>

namespace orka {

OrkaMainWindow::OrkaMainWindow()
{
	mGLImageDisplayWidget = new GLImageDisplayWidget();
//    QTimer *timer = new QTimer(this);
//
//    timer->setInterval(20); // 20 ms = 50 fps.

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

    fileMenu->addAction(exit);
    controlMenu->addAction(togglePlayPause);
    helpMenu->addAction(aboutQt);

    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(togglePlayPause, SIGNAL(triggered(bool)), mGLImageDisplayWidget, SLOT(togglePlayPause()));
    QObject::connect(aboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    mGLImageDisplayWidget->start();
}

void OrkaMainWindow::setImageProvider(ImageProvider * provider) {
	mGLImageDisplayWidget->setImageProvider(provider);
}

} // end namespace orka

