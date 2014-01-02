#include "GLImageDisplayWidget.h"

#include <QPainter>
#include <QPaintEngine>
#include <OpenColorIO/OpenColorIO.h>

#include <math.h>
#include <map>
#include <string>

#include "OrkaImage.h"
#include "ImageProvider.h"
#include "OrkaViewSettings.h"

namespace OCIO = OCIO_NAMESPACE;

namespace orka {

GLImageDisplayWidget::GLImageDisplayWidget(OrkaViewSettings * view_settings,
        QWidget *parent) :
        QGLWidget(parent), view_settings_(view_settings), image_provider_(NULL),
        current_image_(NULL), image_width_(0), image_height_(0),
        image_transferred_(false) {
    gl_update_timer_ = new QTimer(this);
    mouse_drag_status_ = Inactive;
    frame_of_a_hundred_ = 0;

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoBufferSwap(false);

    setMinimumSize(640, 480);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Always enable mouseMove events, not only when the mouse is pressed down.
    setMouseTracking(true);

    QObject::connect(gl_update_timer_, SIGNAL(timeout()), this, SLOT(updateGL()));
    gl_update_timer_->start(20);
}

GLImageDisplayWidget::~GLImageDisplayWidget() {
}

void GLImageDisplayWidget::start() {
    image_provider_->start();
}

void GLImageDisplayWidget::stop() {
    image_provider_->stop();
}

void GLImageDisplayWidget::togglePlayPause() {
    image_provider_->toggleStartStop();
}

void GLImageDisplayWidget::set_image_provider(ImageProvider * provider) {
    image_provider_ = provider;
}

void GLImageDisplayWidget::displayImage(OrkaImage * image, int frame) {
    current_image_ = image;
    image_transferred_ = false;
    loadImage();
}

void GLImageDisplayWidget::fitZoomToWindow() {
    float zoom = 1.0;
    float height = this->height();
    float width = this->width();
    if (image_width_ > image_height_) {
        zoom = width / static_cast<float>(image_width_);
    } else {
        zoom = height / static_cast<float>(image_height_);
    }
    view_settings_->set_zoom(zoom);
    view_settings_->resetTranslation();
}

void GLImageDisplayWidget::loadImage() {
    if (!current_image_ || image_transferred_)
        return;

    image_width_ = current_image_->width();
    image_height_ = current_image_->height();

    GLenum formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
    glActiveTexture(GL_TEXTURE0 + IMAGE_TEX_INDEX);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gl_texture_ids_[IMAGE_TEX_INDEX]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, current_image_->width(),
            current_image_->height(), 0, formats[current_image_->channels()],
            current_image_->glType(), reinterpret_cast<GLvoid *>(current_image_->getPixels()));
    glBindTexture(GL_TEXTURE_2D, 0);
    image_transferred_ = true;
}

QString readFile(QString filename) {
    QFile file(filename);
    QString result;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return result;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        result.append(line + "\n");
    }
    return result;
}

void GLImageDisplayWidget::initializeGL() {
    glGenTextures(2, gl_texture_ids_);

    QString image_fragment_shader_text = readFile("image_fragment.glsl");
    try {
        const std::map<std::string, std::string> colorSpaceMapping = {
                {"Linear", "scene_linear" },
                { "GammaCorrected", "reference" },
                {"sRGB", "reference" },
                { "AdobeRGB", "reference" },
                { "Rec709", "reference" },
                { "KodakLog", "compositing_log" }
        };
        auto color_space_role_iterator = colorSpaceMapping.find(
                image_provider_->getColorSpace());
        std::string ocio_role = "default";
        if (color_space_role_iterator != colorSpaceMapping.end()) {
            ocio_role = color_space_role_iterator->second;
        }
        std::cout << "Color space: " << image_provider_->getColorSpace()
                << " role: " << ocio_role << std::endl;

        OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
        // If the user hasn't picked a display, use the defaults...
        const char * device = config->getDefaultDisplay();
        const char * view = config->getDefaultView(device);
        const char * displayColorSpace = config->getDisplayColorSpaceName(
                device, view);

        OCIO::ConstProcessorRcPtr processor = config->getProcessor(
                ocio_role.c_str(), displayColorSpace);

        OCIO::GpuShaderDesc shaderDesc;
        shaderDesc.setLanguage(OCIO::GPU_LANGUAGE_GLSL_1_0);
        shaderDesc.setFunctionName("OCIODisplay");
        const int LUT3D_EDGE_SIZE = 64;
        shaderDesc.setLut3DEdgeLen(LUT3D_EDGE_SIZE);

        int num3Dentries = 3 * LUT3D_EDGE_SIZE * LUT3D_EDGE_SIZE
                * LUT3D_EDGE_SIZE;

        lut3d_data_.resize(num3Dentries);
        processor->getGpuLut3D(&lut3d_data_[0], shaderDesc);
        const char * lut_shader_text = processor->getGpuShaderText(shaderDesc);
        image_fragment_shader_text.prepend(lut_shader_text);

        glActiveTexture(GL_TEXTURE0 + LUT_TEX_INDEX);
        glEnable(GL_TEXTURE_3D);
        glBindTexture(GL_TEXTURE_3D, gl_texture_ids_[LUT_TEX_INDEX]);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUT3D_EDGE_SIZE, LUT3D_EDGE_SIZE,
                LUT3D_EDGE_SIZE, 0, GL_RGB, GL_FLOAT, &lut3d_data_[0]);
        glBindTexture(GL_TEXTURE_3D, 0);
    } catch (OCIO::Exception & exception) {
        std::cerr << "OpenColorIO Error: " << exception.what() << std::endl;
    }

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    glActiveTexture(GL_TEXTURE0 + IMAGE_TEX_INDEX);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gl_texture_ids_[IMAGE_TEX_INDEX]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // TODO(vilhelmo): Make linear/nearest texture filtering configurable.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    image_program_.removeAllShaders();
    image_program_.addShaderFromSourceFile(QOpenGLShader::Vertex,
            QString("image_vertex.glsl"));
    image_program_.addShaderFromSourceCode(QOpenGLShader::Fragment,
            image_fragment_shader_text);
    image_program_.link();

    image_vertex_attr_ = image_program_.attributeLocation("vertex");
    image_tex_coord_attr_ = image_program_.attributeLocation("uv");
    image_matrix_uniform_ = image_program_.uniformLocation("matrix");
    image_texture_uniform_ = image_program_.uniformLocation("imageSampler");
    image_lutSampler_uniform_ = image_program_.uniformLocation("lutSampler");
    image_exposure_uniform_ = image_program_.uniformLocation("exposure");
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
        int dy = event->y() - mouse_drag_y_;
        if (mouse_drag_status_ == Translate) {
            // scale translation
            view_settings_->move(dx, dy);
        } else if (mouse_drag_status_ == Jog && fabs(dx) >= 10) {
            image_provider_->jog(static_cast<float>(dx) / 10.0);
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

    QMatrix4x4 modelview;
    paintImage(&modelview);

    // Paint other native ui elements.
    paintColorPicker(&painter, modelview);

    painter.endNativePainting();

    QString framesPerSecond;
    framesPerSecond.setNum(frame_of_a_hundred_ / (fps_time_.elapsed() / 1000.0), 'f', 2);

    painter.setPen(Qt::white);

    painter.drawText(20, 40, framesPerSecond + " fps");

    painter.end();

    swapBuffers();

    if (!(frame_of_a_hundred_ % 100)) {
        fps_time_.start();
        frame_of_a_hundred_ = 0;
    }
    frame_of_a_hundred_++;
}

void GLImageDisplayWidget::doPaint(const float * vertices,
        const float * texture_coords, const QMatrix4x4 & transform) {
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_3D, gl_texture_ids_[LUT_TEX_INDEX]);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, gl_texture_ids_[IMAGE_TEX_INDEX]);

    image_program_.bind();

    image_program_.setUniformValue(image_lutSampler_uniform_, LUT_TEX_INDEX);
    image_program_.setUniformValue(image_texture_uniform_, IMAGE_TEX_INDEX);
    image_program_.setUniformValue(image_matrix_uniform_, transform);
    image_program_.setUniformValue(image_exposure_uniform_,
            view_settings_->exposure());
    image_program_.enableAttributeArray(image_vertex_attr_);
    image_program_.setAttributeArray(image_vertex_attr_, vertices, 2);
    image_program_.enableAttributeArray(image_tex_coord_attr_);
    image_program_.setAttributeArray(image_tex_coord_attr_, texture_coords, 2);

    glDrawArrays(GL_QUADS, 0, 4);

    image_program_.disableAttributeArray(image_vertex_attr_);
    image_program_.disableAttributeArray(image_tex_coord_attr_);

    image_program_.release();

    glActiveTexture(GL_TEXTURE0 + IMAGE_TEX_INDEX);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + LUT_TEX_INDEX);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void GLImageDisplayWidget::paintImage(QMatrix4x4 * modelview) {
    float window_width = this->width();
    float window_height = this->height();

//    float max = mImageWidth > mImageHeight ? mImageWidth : mImageHeight;

    // -1 <= x,y <= 1
    float half_window_width = window_width / 2.f;
    float half_window_height = window_height / 2.f;
    modelview->setToIdentity();
    modelview->ortho(-half_window_width, half_window_width,  // left, right
            half_window_height, -half_window_height,  // bottom, top
            0, 1);
    modelview->scale(view_settings_->zoom());

//    // adjust to aspect ratio of image.
//    modelview.scale(1.0 / max, 1.0 / max);
//    // Scale image to pixels on screen.
//    modelview.scale(float(mImageWidth) / width);
//    // adjust aspect ratio of window.
//    modelview.scale(1.0, width / height);
//    modelview.scale(view_settings_->zoom());
//    modelview.translate(2.0 * view_settings_->tx(),
//            -2.0 * view_settings_->ty());

    if (!current_image_) {
        return;
    }

    float half_width = static_cast<float>(image_width_) / 2.0;
    float half_height = static_cast<float>(image_height_) / 2.0;
    float tx = view_settings_->tx();
    float ty = view_settings_->ty();
    float vertices[8] = { -half_width + tx, -half_height + ty,  //
    half_width + tx, -half_height + ty,  //
    half_width + tx, half_height + ty,  //
    -half_width + tx, half_height + ty };
    float textureCoords[8] = { 0.f, 1.f,  //
            1.f, 1.f,  //
            1.f, 0.f,  //
            0.f, 0.f };

    doPaint(vertices, textureCoords, *modelview);
}

void GLImageDisplayWidget::paintColorPicker(QPainter * painter,
        const QMatrix4x4 & image_transform) {
    if (!current_image_) {
        return;
    }

    float starty = this->height() - 25.f;
    float color_picker_screen_coords[8] = { 10.f, starty, 10.f, starty + 15.f,
            25.f, starty + 15.f, 25.f, starty, };

    float norm_mouse_x = 2.0 * mouse_track_x_ / static_cast<float>(this->width()) - 1.0;
    float norm_mouse_y = 2.0 * mouse_track_y_ / static_cast<float>(this->height()) - 1.0;
    bool invertible = true;
    QMatrix4x4 inv_image_transform = image_transform.inverted(&invertible);
    QVector4D pos = inv_image_transform
            * QVector3D(norm_mouse_x, norm_mouse_y, 0);

    float half_width = static_cast<float>(image_width_) / 2.0;
    float half_height = static_cast<float>(image_height_) / 2.0;
    float tx = view_settings_->tx();
    float ty = view_settings_->ty();
    pos += QVector3D(half_width - tx, half_height + ty, 0);

    int x = pos.x();
    int y = image_height_ - pos.y();

    std::stringstream ss;
    ss << "Position (" << x << ", " << y << ")";

    float pixel_color[4] = { 0.f, 0.f, 0.f, 1.f };
    if (x >= 0 && x < image_width_ && y >= 0 && y < image_height_) {
        for (uint i = 0; i < current_image_->channels(); i += 1) {
            pixel_color[i] = current_image_->getPixel(x, y, i);
        }
    }

    float tex_coord_x = x / static_cast<float>(image_width_);
    float tex_coord_y = y / static_cast<float>(image_height_);
    float color_picker_tex_coords[8] = { tex_coord_x, 1.f - tex_coord_y,
            tex_coord_x, 1.f - tex_coord_y, tex_coord_x, 1.f - tex_coord_y,
            tex_coord_x, 1.f - tex_coord_y };

    QMatrix4x4 ortho_matrix;
    ortho_matrix.ortho(this->rect());

    doPaint(color_picker_screen_coords, color_picker_tex_coords, ortho_matrix);

    painter->setPen(Qt::white);
    painter->drawText(30, starty, ss.str().c_str());
    std::stringstream ss2;
    ss2 << "Color: " << pixel_color[0] << ", " << pixel_color[1] << ", "
            << pixel_color[2] << ", " << pixel_color[3];
    painter->setPen(Qt::white);
    painter->drawText(30, starty + 15, ss2.str().c_str());
}

}  // end namespace orka
