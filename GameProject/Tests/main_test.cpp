//#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_RUNNER
#include "Catch/catch.hpp"

#include <Windows.h>
#include <Engine/Rendering/Display.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"

int main(int argc, char* argv[]) {
	// Global setup
	Display::get().init(640, 480, "Tests");

	// Run tests
	int result = Catch::Session().run(argc, argv);

	// Global clean-up
	Display::get().~Display();

	// Keep console open
	std::getchar();

	return result;
}
