#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace orka {

class GLImageDisplayWidget;
class ControlBar;
class ImageProvider;
class OrkaViewSettings;

class OrkaMainWindow: public QMainWindow {
Q_OBJECT

 public:
    OrkaMainWindow(OrkaViewSettings * view_settings);
    void set_image_provider(ImageProvider * provider);
 public slots:
     void toggleFullscreen();

 private:
    QWidget * central_widget_;
    ControlBar * control_bar_;
    GLImageDisplayWidget * image_display_gl_widget_;
};

} // end namespace orka

#endif
