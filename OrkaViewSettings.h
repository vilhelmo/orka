/*
 * OrkaViewSettings.h
 *
 *  Created on: Dec 23, 2013
 *      Author: vilhelm
 */

#ifndef ORKAVIEWSETTINGS_H_
#define ORKAVIEWSETTINGS_H_

#include <QObject>

namespace orka {

class OrkaViewSettings : public QObject {
Q_OBJECT
public:
	OrkaViewSettings(QObject * parent = NULL);
	virtual ~OrkaViewSettings();

	int tx() { return tx_; }
	int ty() { return ty_; }
	float zoom() { return zoom_; }
	float exposure() { return exposure_; }

public slots:
	void zoomIn(float factor = 1.05) {
		zoom_ *= factor;
	}
	void zoomOut() {
		zoom_ /= 1.05;
	}
	void resetZoom() {
		zoom_ = 1.0;
	}
	void resetTranslation() {
		tx_ = ty_ = 0;
	}
	void move(int dx, int dy) {
		tx_ += dx;
		ty_ += dy;
	}
	void increaseExposure() {
		exposure_ *= 2.0;
	}
	void decreaseExposure() {
		exposure_ /= 2.0;
	}
private:
	int tx_;
	int ty_;
	float zoom_;
	float exposure_;
};

} /* namespace orka */
#endif /* ORKAVIEWSETTINGS_H_ */