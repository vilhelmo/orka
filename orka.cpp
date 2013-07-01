/*
 *
 * orka.cpp
 *
 */
#include <iostream>
#include <exception>
#include <string>
#include <OpenImageIO/imageio.h>
#include <GLFW/glfw3.h>

using namespace OpenImageIO;

class OrkaException: virtual public std::exception {
public:
	OrkaException(std::string description) :
			description_(description) {
	}
	virtual ~OrkaException() throw () {
	}
	virtual const char* what() const throw () {
		return this->description_.c_str();
	}
private:
	std::string description_;
};

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

void testGLFW3() {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		throw OrkaException("glfwInit failed.");

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Orka Image Viewer", NULL, NULL);
	if (!window) {
		glfwTerminate();
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Render here */

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
}

int main(int argc, char* argv[]) {
	std::cout << "This is a Linux image viewer!" << std::endl;

	if (argc < 2) {
		std::cerr << "Need an image path." << std::endl;
		return 1;
	}

	char * filename = argv[1];

	try {
		// Verify openimageio working
		testOpenImageIO(filename);
		// Verify glfw is working
		testGLFW3();
		std::cout << "Everything working as expected!" << std::endl;
	} catch (OrkaException e) {
		std::cerr << "Encountered a problem: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}
