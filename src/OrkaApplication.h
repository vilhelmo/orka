/*
 * OrkaApplication.h
 *
 *  Created on: Dec 10, 2013
 *      Author: vilhelm
 */

#ifndef SRC_ORKAAPPLICATION_H_
#define SRC_ORKAAPPLICATION_H_

#include <string>
#include <vector>

namespace orka {

class OrkaMainWindow;
class OrkaViewSettings;
class ImageProvider;

class OrkaApplication {
public:
	OrkaApplication(const std::vector<std::string> & files);
	virtual ~OrkaApplication();
	void showMainWindow();
	OrkaViewSettings * view_settings() { return view_settings_; }
private:
	OrkaMainWindow * main_window_;
	ImageProvider * image_provider_;
	OrkaViewSettings * view_settings_;
};

} /* namespace orka */


#endif  // SRC_ORKAAPPLICATION_H_
