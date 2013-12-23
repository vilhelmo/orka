/*
 * OrkaApplication.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: vilhelm
 */

#include "OrkaApplication.h"
#include "OrkaMainWindow.h"
#include "ImageSequenceProvider.h"

#include <iostream>

namespace orka {

OrkaApplication::OrkaApplication(const std::vector<std::string> & files) {
	std::cout << "Creating window" << std::endl;
	mainWindow = new OrkaMainWindow();
	std::cout << "Creating im provider" << std::endl;
	mProvider = new ImageSequenceProvider(files);
	std::cout << "set im prov" << std::endl;
	mainWindow->setImageProvider(mProvider);
//	std::cout << "m prov start." << std::endl;
//	mProvider->start();
//	std::cout << "m prov started." << std::endl;
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
