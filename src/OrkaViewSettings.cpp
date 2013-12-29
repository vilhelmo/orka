/*
 * OrkaViewSettings.cpp
 *
 *  Created on: Dec 23, 2013
 *      Author: vilhelm
 */

#include "OrkaViewSettings.h"

namespace orka {

OrkaViewSettings::OrkaViewSettings(QObject * parent) : QObject(parent),
		tx_(0), ty_(0), zoom_(1.0), exposure_(1.0), gamma_(2.2) {
}

OrkaViewSettings::~OrkaViewSettings() {
	// TODO Auto-generated destructor stub
}

} /* namespace orka */
