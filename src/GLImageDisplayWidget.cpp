#include "GLImageDisplayWidget.h"

#include <QPainter>
#include <QPaintEngine>
#include <math.h>

#include "OrkaImage.h"
#include "ImageProvider.h"
#include "OrkaViewSettings.h"

namespace orka {

GLImageDisplayWidget::GLImageDisplayWidget(OrkaViewSettings * view_settings,
        QWidget *parent) :
        QGLWidget(parent), view_settings_(view_settings), mImageProvider(NULL), mCurrentImage(
                NULL), mImageWidth(0), mImageHeight(0), image_transferred_(
                false) {
    mGLUpdateTimer = new QTimer(this);
    mouse_drag_status_ = Inactive;
    frames = 0;

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoBufferSwap(false);

    setMinimumSize(640, 480);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Always enable mouseMove events, not only when the mouse is pressed down.
    setMouseTracking(true);

    QObject::connect(mGLUpdateTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    mGLUpdateTimer->start(20);
}

GLImageDisplayWidget::~GLImageDisplayWidget() {
}

void GLImageDisplayWidget::start() {
    mImageProvider->start();
}

void GLImageDisplayWidget::stop() {
    mImageProvider->stop();
}

void GLImageDisplayWidget::togglePlayPause() {
    mImageProvider->toggleStartStop();
}

void GLImageDisplayWidget::setImageProvider(ImageProvider * provider) {
    mImageProvider = provider;
}

void GLImageDisplayWidget::displayImage(OrkaImage * image, int frame) {
    mCurrentImage = image;
    image_transferred_ = false;
}

void GLImageDisplayWidget::fitZoomToWindow() {
    float zoom = 1.0;
    float height = this->height();
    float width = this->width();
    if (mImageWidth > mImageHeight) {
        zoom = width/float(mImageWidth);
    } else {
        zoom = height/float(mImageHeight);
    }
    view_settings_->set_zoom(zoom);
    view_settings_->resetTranslation();
}

void GLImageDisplayWidget::loadImage() {
    if (!mCurrentImage || image_transferred_)
        return;

//    std::cout << "Load image..." << std::endl;
//	assert(mCurrentImage->channels() == 3); // TODO: Support != 3 rgb channels
    mImageWidth = mCurrentImage->width();
    mImageHeight = mCurrentImage->height();

    glBindTexture(GL_TEXTURE_2D, gl_image_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mCurrentImage->width(),
            mCurrentImage->height(), 0, GL_RGB, mCurrentImage->glType(),
            (GLvoid *) mCurrentImage->getPixels());
    glBindTexture(GL_TEXTURE_2D, 0);
    image_transferred_ = true;
}

void GLImageDisplayWidget::initializeGL() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &gl_image_tex_id);
    glBindTexture(GL_TEXTURE_2D, gl_image_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // TODO: Make linear/nearest texture filtering configurable.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    image_program_.removeAllShaders();
    image_program_.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString("image_vertex.glsl"));
    image_program_.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString("image_fragment.glsl"));
    image_program_.link();

    image_vertex_attr_ = image_program_.attributeLocation("vertex");
    image_tex_coord_attr_ = image_program_.attributeLocation("uv");
    image_matrix_uniform_ = image_program_.uniformLocation("matrix");
    image_texture_uniform_ = image_program_.uniformLocation("imageSampler");
    image_exposure_uniform_ = image_program_.uniformLocation("exposure");

    default_program_.removeAllShaders();
    default_program_.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString("default_vertex.glsl"));
    default_program_.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString("default_fragment.glsl"));
    default_program_.link();

    default_vertex_attr_ = default_program_.attributeLocation("vertex");
    default_color_uniform_ = default_program_.uniformLocation("color");
    default_matrix_uniform_ = default_program_.uniformLocation("matrix");
}

void GLImageDisplayWidget::mousePressEvent(QMouseEvent * event) {
    event->accept();
    Qt::MouseButton b = event->button();
    if (b == Qt::RightButton) {
        mouse_drag_status_ = Jog;
    } else {
        mouse_drag_status_ = Translate;
    }
    mouse_drag_x_ = event->x();
    mouse_drag_y_ = event->y();
}

void GLImageDisplayWidget::mouseMoveEvent(QMouseEvent * event) {
    event->accept();
    if (mouse_drag_status_ != Inactive) {
        int dx = event->x() - mouse_drag_x_;
        int dy = event->y() - mouse_drag_y_; // go other way in y.
        if (mouse_drag_status_ == Translate) {
            // scale translation
            view_settings_->move(dx, dy);
        } else if (mouse_drag_status_ == Jog && fabs(dx) >= 10) {
            mImageProvider->jog(float(dx) / 10.0);
        } else if (mouse_drag_status_ == Jog) {
            return;
        }
        mouse_drag_x_ = event->x();
        mouse_drag_y_ = event->y();
    } else {
        mouse_track_x_ = event->x();
        mouse_track_y_ = event->y();
    }
}

void GLImageDisplayWidget::mouseReleaseEvent(QMouseEvent * event) {
    event->accept();
    mouse_drag_status_ = Inactive;
}

void GLImageDisplayWidget::wheelEvent(QWheelEvent * event) {
    event->accept();
    float numDegrees = event->delta() / 8.0;
    float numSteps = numDegrees / 15.0;
    view_settings_->zoomIn(pow(1.05, numSteps));
}

void GLImageDisplayWidget::paintGL() {
    QPainter painter;
    painter.begin(this);

    painter.beginNativePainting();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    loadImage();

    float height = this->height();
    float width = this->width();

    float max = mImageWidth > mImageHeight ? mImageWidth : mImageHeight;

    // -1 <= x,y <= 1
    float half_width = width/2.f;
    float half_height = height/2.f;
    QMatrix4x4 modelview;
    modelview.ortho(-half_width, half_width, // left, right
            half_height, -half_height, // bottom, top
            0, 1);
    modelview.scale(view_settings_->zoom());

//    // adjust to aspect ratio of image.
//    modelview.scale(1.0 / max, 1.0 / max);
//    // Scale image to pixels on screen.
//    modelview.scale(float(mImageWidth) / width);
//    // adjust aspect ratio of window.
//    modelview.scale(1.0, width / height);
//    modelview.scale(view_settings_->zoom());
//    modelview.translate(2.0 * view_settings_->tx(),
//            -2.0 * view_settings_->ty());

    image_program_.bind();
    image_program_.setUniformValue(image_matrix_uniform_, modelview);
    image_program_.setUniformValue(image_exposure_uniform_, view_settings_->exposure());
    paintImage();
    image_program_.release();

    // Paint other native ui elements.
    paintColorPicker(painter, modelview);

    painter.endNativePainting();

    QString framesPerSecond;
    framesPerSecond.setNum(frames / (time.elapsed() / 1000.0), 'f', 2);

    painter.setPen(Qt::white);

    painter.drawText(20, 40, framesPerSecond + " fps");

    painter.end();

    swapBuffers();

    if (!(frames % 100)) {
        time.start();
        frames = 0;
    }
    frames++;
}

void GLImageDisplayWidget::paintImage() {
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_image_tex_id);

    float half_width = float(mImageWidth)/2.0;
    float half_height = float(mImageHeight)/2.0;
    float tx = view_settings_->tx()/view_settings_->zoom();
    float ty = view_settings_->ty()/view_settings_->zoom();
    float vertices[8] = { -half_width+tx, -half_height+ty, //
            half_width+tx, -half_height+ty, //
            half_width+tx, half_height+ty, //
            -half_width+tx, half_height+ty };
    float textureCoords[8] = { 0.f, 1.f, //
            1.f, 1.f, //
            1.f, 0.f, //
            0.f, 0.f };

    image_program_.enableAttributeArray(image_vertex_attr_);
    image_program_.setAttributeArray(image_vertex_attr_, vertices, 2);
    image_program_.enableAttributeArray(image_tex_coord_attr_);
    image_program_.setAttributeArray(image_tex_coord_attr_, textureCoords, 2);

    image_program_.setUniformValue(image_texture_uniform_, 0); // use texture unit 0

    glDrawArrays(GL_QUADS, 0, 4);

    image_program_.disableAttributeArray(image_vertex_attr_);
    image_program_.disableAttributeArray(image_tex_coord_attr_);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLImageDisplayWidget::paintColorPicker(QPainter & painter, QMatrix4x4 & image_transform) {
    if (!mCurrentImage) {
        return;
    }

    float starty = this->height() - 25.f;
    float color_picker_screen_coords [8] = {
            10.f, starty,
            10.f, starty+15.f,
            25.f, starty+15.f,
            25.f, starty,
    };

    float norm_mouse_x = 2.0*mouse_track_x_/float(this->width()) - 1.0;
    float norm_mouse_y = 2.0*mouse_track_y_/float(this->height()) - 1.0;
    bool invertible = true;
    QMatrix4x4 inv_image_transform = image_transform.inverted(&invertible);
    QVector4D pos = inv_image_transform * QVector3D(norm_mouse_x, norm_mouse_y, 0);

    float half_width = float(mImageWidth)/2.0;
    float half_height = float(mImageHeight)/2.0;
    float tx = view_settings_->tx();
    float ty = view_settings_->ty();
    pos += QVector3D(half_width-tx, half_height+ty, 0);

    int x = pos.x();
    int y = mImageHeight - pos.y();

    std::stringstream ss;
    ss << "Position (" << x << ", " << y << ")";

    float pixel_color[4] = { 0.f, 0.f, 0.f, 1.f };
    if (x >= 0 && x < mImageWidth &&
            y >= 0 && y < mImageHeight) {
        for (uint i = 0; i < mCurrentImage->channels(); i += 1) {
            pixel_color[i] = mCurrentImage->getPixel(x, y, i);
        }
    }


    default_program_.bind();
    QMatrix4x4 ortho_matrix;
    ortho_matrix.ortho(this->rect());
    default_program_.setUniformValue(default_matrix_uniform_, ortho_matrix);

    default_program_.setUniformValue(default_color_uniform_, pixel_color[0],
            pixel_color[1], pixel_color[2], pixel_color[3]);

    default_program_.enableAttributeArray(default_vertex_attr_);
    default_program_.setAttributeArray(default_vertex_attr_, color_picker_screen_coords, 2);

    glDrawArrays(GL_QUADS, 0, 4);

    default_program_.disableAttributeArray(default_vertex_attr_);
    default_program_.release();

    painter.setPen(Qt::white);
    painter.drawText(30, starty, ss.str().c_str());
    std::stringstream ss2;
    ss2 << "Color: " << pixel_color[0] << ", " << pixel_color[1] << ", "
            << pixel_color[2] << ", " << pixel_color[3];
    painter.setPen(Qt::white);
    painter.drawText(30, starty+15, ss2.str().c_str());
}

} // end namespace orka
