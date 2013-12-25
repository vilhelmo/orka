/*
 * OrkaApplication.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: vilhelm
 */

#include "OrkaApplication.h"
#include "OrkaMainWindow.h"
#include "OrkaViewSettings.h"
#include "OrkaException.h"
#include "ImageSequenceProvider.h"
#include "VLCMovieProvider.h"

#include <iostream>
#include <OpenImageIO/imageio.h>

namespace orka {

class ImageProviderFactory {
public:
	static ImageProvider * createProvider(const std::vector<std::string> & files) {
		if (files.size() == 0) {
			throw new OrkaException("No files provided.");
		}
		OpenImageIO::ImageInput * first_input = OpenImageIO::ImageInput::create(files.at(0));
		if (files.size() > 1 || first_input != NULL) {
			return new ImageSequenceProvider(files);
		} else if (files.size() == 1) {
			return new VLCMovieProvider(files.at(0));
		} else {
			throw new OrkaException("Only a single movie at a time supported right now");
		}
	}
};

OrkaApplication::OrkaApplication(const std::vector<std::string> & files) {
	view_settings_ = new OrkaViewSettings();
	mProvider = ImageProviderFactory::createProvider(files);
	mainWindow = new OrkaMainWindow(view_settings_);
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
