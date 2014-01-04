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
#include "ImageCacheProvider.h"
#include "VLCMovieProvider.h"

#include <OpenImageIO/imageio.h>

#include <iostream>

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
	image_provider_ = ImageProviderFactory::createProvider(files);
	main_window_ = new OrkaMainWindow(view_settings_);
	main_window_->set_image_provider(image_provider_);
}

OrkaApplication::~OrkaApplication() {
    main_window_->hide();
	delete main_window_;
	delete image_provider_;
	delete view_settings_;
}

void OrkaApplication::showMainWindow() {
	main_window_->show();
}

} /* namespace orka */
