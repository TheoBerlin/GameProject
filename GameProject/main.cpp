#include <Windows.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

//Testing please remove
#include "EntityMatrix.h"

#include <iostream>
#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Game/Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h" //Single library for img loader

//Error Handling
void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//Close window on ESC
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
	//INIT
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	glfwSetErrorCallback(error_callback);

	EntityMatrix eMatrix;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << eMatrix.getMatrix()[i][j] << " : ";
		}
		std::cout << std::endl;
	}

	eMatrix.translate(glm::vec3(2, 0, 0));
	eMatrix.rotate(glm::vec3(1, -2, 1), glm::vec3(0, 0, 0));
	std::cout << std::endl;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << eMatrix.getMatrix()[i][j] << " : ";
		}
		std::cout << std::endl;
	}

	std::cout << std::endl << eMatrix.getPosition().x << " " << eMatrix.getPosition().y << " " << eMatrix.getPosition().z;

	if (!glfwInit())
	{
		// Initialization failed
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window)
	{
		// Window or context creation failed
	}

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();

	//Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwSetKeyCallback(window, key_callback);
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		Game game;
		game.start();
		*/

		glfwSwapBuffers(window);
		glfwWaitEvents();
	}
	
	//Delete
	glfwDestroyWindow(window);
	glfwTerminate();	
	Logger::destroy();
	return 0;
}
