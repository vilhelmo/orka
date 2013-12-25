#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace orka {

class GLImageDisplayWidget;
class ImageProvider;
class OrkaViewSettings;

class OrkaMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    OrkaMainWindow(OrkaViewSettings * view_settings);
	void setImageProvider(ImageProvider * provider);
private:
    GLImageDisplayWidget * mGLImageDisplayWidget;
};

} // end namespace orka

#endif
