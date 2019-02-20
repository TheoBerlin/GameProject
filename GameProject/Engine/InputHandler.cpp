#include "InputHandler.h"

#include "Events/EventBus.h"
#include "Utils/Logger.h"

double InputHandler::lastPosX = 0.0;
double InputHandler::lastPosY = 0.0;

// action = [GLFW_PRESS, GLFW_RELEASE]
void InputHandler::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_REPEAT)
	{
		EventBus::get().publish(&KeyEvent(key, action));
	}
}

void InputHandler::mouseMoveCallback(GLFWwindow * window, double xpos, double ypos)
{
	// Calculate travel distance
	double travelDistanceX = xpos - lastPosX;
	double travelDistanceY = ypos - lastPosY;
	lastPosX = xpos;
	lastPosY = ypos;

	// Send event when mouse is moved to EventBus
	EventBus::get().publish(&MouseMoveEvent(xpos, ypos, travelDistanceX, travelDistanceY));
}

// action = [GLFW_PRESS, GLFW_RELEASE]
void InputHandler::mouseClickCallback(GLFWwindow * window, int button, int action, int mods)
{
	// Send event when mouse is clicked to EventBus
	EventBus::get().publish(&MouseClickEvent(button, action));
}

InputHandler::InputHandler(GLFWwindow* window)
{
	glfwSetKeyCallback(window, this->keyCallback);
	glfwSetCursorPosCallback(window, this->mouseMoveCallback);
	glfwSetMouseButtonCallback(window, this->mouseClickCallback);
}


InputHandler::~InputHandler()
{
}
