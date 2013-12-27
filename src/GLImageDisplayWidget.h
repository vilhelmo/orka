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
    void fitZoomToWindow();
protected:
    void loadImage();
    void initializeGL();

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);
private:
    void paintGL();
    void paintImage();
    void paintColorPicker(QPainter & painter, QMatrix4x4 & image_transform);

    OrkaViewSettings * view_settings_;
    MouseDragStatus mouse_drag_status_;
    int mouse_drag_x_, mouse_drag_y_;

    ImageProvider * mImageProvider;
    OrkaImage * mCurrentImage;
    int mImageWidth;
    int mImageHeight;
    bool image_transferred_;

    QTimer * mGLUpdateTimer;

    int frames;
    QTime time;

    //===
    QOpenGLShaderProgram image_program_;
    GLuint gl_image_tex_id;
    int image_vertex_attr_, image_tex_coord_attr_;
    int image_matrix_uniform_, image_texture_uniform_;
    int image_exposure_uniform_;
    //===
    QOpenGLShaderProgram default_program_;
    int default_vertex_attr_;
    int default_color_uniform_;
    int default_matrix_uniform_;
    //===

    int mouse_track_x_, mouse_track_y_;
};

} // end namespace orka

#endif
