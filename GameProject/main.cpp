#include <Windows.h>
#include <gl/GLU.h>
#include <glfw3.h>

#include <iostream>
#include <crtdbg.h>

#include "Game/Game.h"

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

	Game game;
	game.start();

	//Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwSetKeyCallback(window, key_callback);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwWaitEvents();
	}
	
	//Delete
	glfwDestroyWindow(window);
	glfwTerminate();	
  return 0;
}
