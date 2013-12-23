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

#include "actions/OrkaAction.h"

namespace orka {

class OrkaImage;
class ImageProvider;

class GLImageDisplayWidget: public QGLWidget {
Q_OBJECT
public:
	GLImageDisplayWidget(QWidget *parent = 0);
	~GLImageDisplayWidget();
public slots:
	void start() {
		mImageTimer->singleShot(41, this, SLOT(fetchImage()));
		mRunning = true;
	}
	void stop() {
		mImageTimer->stop();
		mRunning = false;
	}
	void togglePlayPause() {
		if (mRunning) {
			stop();
		} else {
			start();
		}
	}
	void setImageProvider(ImageProvider * provider);
protected slots:
	void fetchImage();
protected:
	void loadImage();
	void paintGL();
	void initializeGL();

	void mousePressEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void wheelEvent(QWheelEvent * event) {
//		std::cout << "scroll x" << dx << " y " << dy << std::endl;
		event->accept();
		float numDegrees = event->delta() / 8.0;
		float numSteps = numDegrees / 15.0;
		zoom *= pow(1.05, numSteps);
	}
private:
	void paintImage();

	bool mRunning;

	ImageProvider * mImageProvider;
	OrkaImage * mCurrentImage;
	int mImageWidth;
	int mImageHeight;
	QTimer * mImageTimer;
	QMutex mImageMutex;

	QTimer * mGLUpdateTimer;

	int frames;
	QTime time;

	QOpenGLShaderProgram program;
	GLuint m_imageTexture;

	int vertexAttr;
	int matrixUniform;
	int texCoordAttr;
	int textureUniform;
	//====
	int mMouseX, mMouseY;
	int tx, ty;
	float zoom;
};

} // end namespace orka

#endif
