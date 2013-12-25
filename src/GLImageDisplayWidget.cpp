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
    mMouseX = -1;
    mMouseY = -1;
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoBufferSwap(false);
    setMinimumSize(640, 480);
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

void GLImageDisplayWidget::displayImage(OrkaImage * image) {
    mCurrentImage = image;
    image_transferred_ = false;
}

void GLImageDisplayWidget::loadImage() {
    if (!mCurrentImage || image_transferred_)
        return;

//	assert(mCurrentImage->channels() == 3); // TODO: Support != 3 rgb channels
    mImageWidth = mCurrentImage->width();
    mImageHeight = mCurrentImage->height();

    glBindTexture(GL_TEXTURE_2D, m_imageTexture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, im->width(), im->height(), 0, GL_RGBA, GL_FLOAT, (GLvoid *) im->getPixels());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mCurrentImage->width(),
            mCurrentImage->height(), 0, GL_RGB, mCurrentImage->glType(),
            (GLvoid *) mCurrentImage->getPixels());
    glBindTexture(GL_TEXTURE_2D, 0);
    image_transferred_ = true;
}

void GLImageDisplayWidget::paintImage() {
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_imageTexture);

    float halfWidth = float(mImageWidth);
    float halfHeight = float(mImageHeight);
    float vertices[8] = { -halfWidth, -halfHeight, halfWidth, -halfHeight,
            halfWidth, halfHeight, -halfWidth, halfHeight };
    float textureCoords[8] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };

    program.enableAttributeArray(vertexAttr);
    program.setAttributeArray(vertexAttr, vertices, 2);
    program.enableAttributeArray(texCoordAttr);
    program.setAttributeArray(texCoordAttr, textureCoords, 2);

    program.setUniformValue(textureUniform, 0); // use texture unit 0

    glDrawArrays(GL_QUADS, 0, 4);

    program.disableAttributeArray(vertexAttr);
    program.disableAttributeArray(texCoordAttr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLImageDisplayWidget::initializeGL() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_imageTexture);
    glBindTexture(GL_TEXTURE_2D, m_imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // TODO: Make linear/nearest texture filtering configurable.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    program.removeAllShaders();
    program.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString("vertex.glsl"));
    program.addShaderFromSourceFile(QOpenGLShader::Fragment,
            QString("fragment.glsl"));
    program.link();

    vertexAttr = program.attributeLocation("vertex");
    texCoordAttr = program.attributeLocation("uv");
    matrixUniform = program.uniformLocation("matrix");
    textureUniform = program.uniformLocation("imageSampler");
    exposureUniform = program.uniformLocation("exposure");
}

void GLImageDisplayWidget::mousePressEvent(QMouseEvent * event) {
    event->accept();
    Qt::MouseButton b = event->button();
    if (b == Qt::RightButton) {
        mouse_drag_status_ = Jog;
    } else {
        mouse_drag_status_ = Translate;
    }
    mMouseX = event->x();
    mMouseY = event->y();
}

void GLImageDisplayWidget::mouseMoveEvent(QMouseEvent * event) {
    if (mouse_drag_status_ != Inactive) {
        event->accept();
        int dx = event->x() - mMouseX;
        int dy = event->y() - mMouseY;
        if (mouse_drag_status_ == Translate) {
            view_settings_->move(dx, dy);
        } else if (mouse_drag_status_ == Jog && fabs(dx) >= 10) {
            mImageProvider->jog(float(dx) / 10.0);
        } else if (mouse_drag_status_ == Jog) {
            return;
        }
        mMouseX = event->x();
        mMouseY = event->y();
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
    QMatrix4x4 modelview;

    // adjust to aspect ratio of image.
    modelview.scale(1.0 / max, 1.0 / max);

    // Scale image to pixels on screen.
    modelview.scale(float(mImageWidth) / width);

    // adjust aspect ratio of window.
    modelview.scale(1.0, width / height);

    modelview.scale(view_settings_->zoom());

    modelview.translate(2.0 * view_settings_->tx(),
            -2.0 * view_settings_->ty());

    program.bind();
    program.setUniformValue(matrixUniform, modelview);
    program.setUniformValue(exposureUniform, view_settings_->exposure());
    paintImage();
    program.release();

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

} // end namespace orka
