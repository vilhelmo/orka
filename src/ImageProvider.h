/*
 * ImageProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef IMAGEPROVIDER_H_
#define IMAGEPROVIDER_H_

#include <QObject>

namespace orka {

class OrkaImage;

class ImageTimeStruct {
public:
	OrkaImage * image;
	int displayTimeMs;
};

class ImageProvider : public QObject {
	Q_OBJECT
public:
	ImageProvider();
	virtual ~ImageProvider();
public slots:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void toggleStartStop() = 0;
signals:
	void displayImage(OrkaImage * image);
protected:
};

} // end namespace orka


#endif /* IMAGEPROVIDER_H_ */
