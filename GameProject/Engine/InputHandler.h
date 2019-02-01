#pragma once

#include <GLFW/glfw3.h>

class InputHandler
{
private:
	GLFWwindow* window;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);
public:
	InputHandler(GLFWwindow* window);
	virtual ~InputHandler();
};

