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

#include <QApplication>

#include "OrkaApplication.h"
#include "OrkaException.h"

using namespace orka;

std::vector<std::string> parseFiles(int argc, char * argv[]) {
	std::vector<std::string> files;

	int bufsize = 1000;
	char * buffer = (char *) malloc(bufsize * sizeof(char));

	for (int i = 1; i < argc; i += 1) {
		char * file = argv[i];
		std::stringstream cmd;
		cmd << "ls " << file;
//		cmd << "ls \"`find \"" << file << "\" -type f`\"";
//		std::cout << "cmd: " << cmd.str() << std::endl;
		FILE * output = popen(cmd.str().c_str(), "r");
		if(!output || feof(output) || ferror(output)) {
			// bad/
			throw new OrkaException("Blah, couldn't figure out which files to open.");
		}

		while (!feof(output) && !ferror(output)) {
			char * success_str = fgets(buffer, bufsize * sizeof(char), output);
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
	std::cout << "This is orka - the Linux image viewer/player!" << std::endl;

	if (argc < 2) {
		std::cerr << "Need an image path." << std::endl;
		return 1;
	}

	std::vector<std::string> files = parseFiles(argc, argv);
	std::cout << "Opening file(s):";
	for (std::string file : files) {
		std::cout << " " << file;
	}
	std::cout << std::endl;

	try {
		Q_INIT_RESOURCE(texture);
		QApplication a(argc, argv);
		OrkaApplication app(files);
		app.showMainWindow();
		int success = a.exec();
		return success;
	} catch (OrkaException * e) {
		std::cerr << e->what() << std::endl;
		return 1;
	}
}
