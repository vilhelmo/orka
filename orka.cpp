/*
 *
 * orka.cpp
 *
 */
#include <iostream>
#include <OpenImageIO/imageio.h>

using namespace OpenImageIO;

int main(int argc, char* argv[]){
	std::cout << "This is a Linux image viewer!" << std::endl;

	if (argc < 2) {
		std::cerr << "Need an image path." << std::endl;
		return 1;
	}

	char * filename = argv[1];
	std::cout << "Reading image: " << filename << std::endl;
	ImageInput *in = ImageInput::open(filename);
	if (!in) {
		return 1;
	}
	const ImageSpec &spec = in->spec();
	int xres = spec.width;
	int yres = spec.height;
	int channels = spec.nchannels;
	std::vector<unsigned char> pixels (xres*yres*channels);
	in->read_image(TypeDesc::UINT8, &pixels[0]);
	in->close();

	std::cout << "Image size " << xres << "x" << yres << std::endl;
	std::cout << channels << " channels" << std::endl;


	delete in;
}
