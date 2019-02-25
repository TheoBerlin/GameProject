#include "InputHandler.h"

#include "Events/EventBus.h"

int InputHandler::lastPosX = 0;
int InputHandler::lastPosY = 0;

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
	int travelDistanceX = (int)xpos - lastPosX;
	int travelDistanceY = (int)ypos - lastPosY;
	lastPosX = (int)xpos;
	lastPosY = (int)ypos;

	// Send event when mouse is moved to EventBus
	EventBus::get().publish(&MouseMoveEvent((int)xpos, (int)ypos, travelDistanceX, travelDistanceY));
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
