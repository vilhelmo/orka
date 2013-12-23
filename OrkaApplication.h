/*
 * OrkaApplication.h
 *
 *  Created on: Dec 10, 2013
 *      Author: vilhelm
 */

#ifndef ORKAAPPLICATION_H_
#define ORKAAPPLICATION_H_

#include <string>
#include <vector>

namespace orka {

class OrkaMainWindow;
class ImageSequenceProvider;

class OrkaApplication {
public:
	OrkaApplication(const std::vector<std::string> & files);
	virtual ~OrkaApplication();
	void showMainWindow();
private:
	OrkaMainWindow * mainWindow;
	ImageSequenceProvider * mProvider;
};

} /* namespace orka */
#endif /* ORKAAPPLICATION_H_ */
