#ifndef SRC_GLIMAGEDISPLAYWIDGET_H_
#define SRC_GLIMAGEDISPLAYWIDGET_H_

#include <OpenImageIO/imageio.h>

#include <QGLWidget>
#include <QMatrix4x4>
#include <QtOpenGL>
#include <QTime>
#include <QTimer>
#include <QMutex>

#include <cmath>
#include <vector>

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
    void set_image_provider(ImageProvider * provider);

 protected slots:
    void displayImage(OrkaImage * image, int frame, bool freeOldImageData);
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
    void doPaint(const float * vertices, const float * texture_coords,
            GLsizei count, const QMatrix4x4 & transform);
    void paintImage(QMatrix4x4 * modelview);
    void paintColorPicker(QPainter * painter, const QMatrix4x4 & image_transform);

    OrkaViewSettings * view_settings_;
    MouseDragStatus mouse_drag_status_;
    int mouse_drag_x_, mouse_drag_y_;
    int mouse_track_x_, mouse_track_y_;

    ImageProvider * image_provider_;
    OrkaImage * current_image_;
    int image_width_;
    int image_height_;
    bool image_transferred_;

    QTimer * gl_update_timer_;

    int frame_of_a_hundred_;
    QTime fps_time_;

    std::vector<float> lut3d_data_;

    QOpenGLShaderProgram image_program_;
    static const int LUT_TEX_INDEX = 0;
    static const int IMAGE_TEX_INDEX = 1;
    GLuint gl_texture_ids_[2];
    int image_vertex_attr_;
    int image_tex_coord_attr_;

    int image_matrix_uniform_;
    int image_texture_uniform_;
    int image_lutSampler_uniform_;
    int image_exposure_uniform_;
};

}  // end namespace orka

#endif  // SRC_GLIMAGEDISPLAYWIDGET_H_
