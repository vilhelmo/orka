#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <cmath>

#include <QGLWidget>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
//#include <QtOpenGL/qglshaderprogram.h>
#include <QtOpenGL>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <QMutex>

#include <OpenImageIO/imageio.h>

namespace orka {

class OrkaImage;
class ImageProvider;
class OrkaViewSettings;

enum MouseDragStatus {
    Inactive = 0, Translate = 1, Jog = 2
};

class GLImageDisplayWidget: public QGLWidget {
Q_OBJECT
public:
    GLImageDisplayWidget(OrkaViewSettings * view_settings, QWidget *parent = 0);
    ~GLImageDisplayWidget();
public slots:
    void start();
    void stop();
    void togglePlayPause();
    void setImageProvider(ImageProvider * provider);
protected slots:
    void displayImage(OrkaImage * image);
protected:
    void loadImage();
    void paintGL();
    void initializeGL();

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);
private:
    void paintImage();

    OrkaViewSettings * view_settings_;
    MouseDragStatus mouse_drag_status_;

    ImageProvider * mImageProvider;
    OrkaImage * mCurrentImage;
    int mImageWidth;
    int mImageHeight;
    bool image_transferred_;

    QTimer * mGLUpdateTimer;

    int frames;
    QTime time;

    QOpenGLShaderProgram program;
    GLuint m_imageTexture;

    int vertexAttr;
    int matrixUniform;
    int texCoordAttr;
    int textureUniform;
    int exposureUniform;
    //====
    int mMouseX, mMouseY;
};

} // end namespace orka

#endif
