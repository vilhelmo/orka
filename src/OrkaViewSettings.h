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
	void set_tx(int tx) { tx_ = tx; }
	int ty() { return ty_; }
	void set_ty(int ty) { ty_ = ty; }
	float zoom() { return zoom_; }
	void set_zoom(float zoom) { zoom_ = zoom; }
	float exposure() { return exposure_; }
	float gamma() { return gamma_; }
	void set_gamma(float gamma) { gamma_ = gamma; }
public slots:
	void zoomIn(float factor = 1.05) {
		zoom_ *= factor;
	}
	void zoomOut() {
		zoom_ /= 1.05;
	}
	void resetZoom() {
		zoom_ = 1.0;
		resetTranslation();
	}
	void resetTranslation() {
		tx_ = ty_ = 0;
	}
	void move(int dx, int dy) {
		tx_ += dx / zoom_;
		ty_ += dy / zoom_;
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
	float gamma_;
};

} /* namespace orka */
#endif /* ORKAVIEWSETTINGS_H_ */
