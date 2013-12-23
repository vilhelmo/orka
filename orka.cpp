/*
 *
 * orka.cpp
 *
 */
#include <iostream>
#include <exception>
#include <string>
#include <sstream>

#include <cstdio>

//#include <glib-2.0/glib.h>

#include <OpenImageIO/imageio.h>
#include <QApplication>
#include <QMainWindow>

#include "OrkaApplication.h"
#include "OrkaException.h"

using namespace OpenImageIO;

using namespace orka;

void testOpenImageIO(char * filename) {
	std::cout << "Reading image: " << filename << std::endl;
	ImageInput *in = ImageInput::open(filename);
	if (!in) {
		throw OrkaException(std::string("Unable to open image: ") + filename);
	}
	const ImageSpec &spec = in->spec();
	int xres = spec.width;
	int yres = spec.height;
	int channels = spec.nchannels;
	std::vector<unsigned char> pixels(xres * yres * channels);
	in->read_image(TypeDesc::UINT8, &pixels[0]);
	in->close();

	std::cout << "Image size " << xres << "x" << yres << std::endl;
	std::cout << channels << " channels" << std::endl;

	delete in;
}

std::vector<std::string> parseFiles(int argc, char * argv[]) {
	std::vector<std::string> files;

	int bufsize = 1000;
	char * buffer = (char *) malloc(bufsize * sizeof(char));

	for (int i = 1; i < argc; i += 1) {
		char * file = argv[i];
		std::stringstream cmd;
		cmd << "ls " << file;
//		std::cout << "cmd: " << cmd.str() << std::endl;
		FILE * output = popen(cmd.str().c_str(), "r");
		if(!output || feof(output) || ferror(output)) {
			// bad/
			throw new OrkaException("Blah, couldn't run ls to figure out files.");
		}

		while (!feof(output) && !ferror(output)) {
//	    	char *fgets( char *restrict str, int count, FILE *restrict stream );
			char * success_str = fgets(buffer, bufsize * sizeof(buffer), output);
			if (!success_str) {
				break; // hmm.
			}
			std::string filename(success_str);
			filename.erase(filename.find_last_not_of(" \n\r\t")+1);
	    	files.push_back(filename);
		}
	}
	free(buffer);
	return files;
}

int main(int argc, char* argv[]) {
	std::cout << "This is a Linux image viewer!" << std::endl;

	if (argc < 2) {
		std::cerr << "Need an image path." << std::endl;
		return 1;
	}

	std::vector<std::string> files = parseFiles(argc, argv);
	for (std::string file : files) {
		std::cout << "File: " << file << std::endl;
	}

//	try {
//		// Verify openimageio working
//		testOpenImageIO(filename);
//		std::cout << "Everything working as expected!" << std::endl;
//	} catch (OrkaException e) {
//		std::cerr << "Encountered a problem: " << e.what() << std::endl;
//		return -1;
//	}
//	std::vector<std::string> files = { std::string(argv[1]) };

	Q_INIT_RESOURCE(texture);
	QApplication a(argc, argv);
	OrkaApplication app(files);
	app.showMainWindow();
	return a.exec();
}
