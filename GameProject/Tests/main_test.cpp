//#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_RUNNER
#include "Catch/catch.hpp"

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"

int main(int argc, char* argv[]) {
	// Global setup
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	GLFWwindow * window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window) {
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}

	// Run tests
	int result = Catch::Session().run(argc, argv);

	// Global clean-up
	glfwDestroyWindow(window);

	// Keep console open
	std::getchar();

	return result;
}
