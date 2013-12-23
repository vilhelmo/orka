/*
 * ImageProvider.h
 *
 *  Created on: Dec 19, 2013
 *      Author: vilhelm
 */

#ifndef IMAGEPROVIDER_H_
#define IMAGEPROVIDER_H_

namespace orka {

class OrkaImage;

class ImageTimeStruct {
public:
	OrkaImage * image;
	int displayTimeMs;
};

class ImageProvider {
public:
	ImageProvider() {}
	virtual ~ImageProvider() {}
	virtual ImageTimeStruct getImage() = 0;

protected:
};

} // end namespace orka


#endif /* IMAGEPROVIDER_H_ */
