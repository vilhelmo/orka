/*
 * ImageProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef IMAGEPROVIDER_H_
#define IMAGEPROVIDER_H_

#include <utility>
#include <string>
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
	virtual std::pair<int, int> getFramerange() = 0;
    virtual std::string getColorSpace() = 0;

public slots:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void toggleStartStop() = 0;
	virtual void jog(int dframes) = 0;
	virtual void gotoFrame(int frame) = 0;
signals:
	void displayImage(OrkaImage * image, int frame);
protected:
};

} // end namespace orka


#endif /* IMAGEPROVIDER_H_ */
