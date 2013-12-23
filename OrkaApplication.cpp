/*
 * OrkaApplication.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: vilhelm
 */

#include "OrkaApplication.h"
#include "OrkaMainWindow.h"
#include "OrkaViewSettings.h"
#include "ImageSequenceProvider.h"

#include <iostream>

namespace orka {

OrkaApplication::OrkaApplication(const std::vector<std::string> & files) {
	view_settings_ = new OrkaViewSettings();
	mainWindow = new OrkaMainWindow(view_settings_);
	mProvider = new ImageSequenceProvider(files);
	mainWindow->setImageProvider(mProvider);
}

OrkaApplication::~OrkaApplication() {
	mainWindow->hide();
	delete mainWindow;
	delete mProvider;
}

void OrkaApplication::showMainWindow() {
	std::cout << "show main window" << std::endl;
	mainWindow->show();
}

} /* namespace orka */
