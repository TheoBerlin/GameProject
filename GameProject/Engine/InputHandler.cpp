#include "InputHandler.h"

#include "Events/EventBus.h"

//action = [GLFW_PRESS, GLFW_REPEAT, GLFW_RELEASE]
void InputHandler::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	EventBus::get().publish(&KeyEvent(key, action));
}

void InputHandler::mouseMoveCallback(GLFWwindow * window, double xpos, double ypos)
{
	//Send event when mouse is moved to EventBus
	//EventBus::get().publish(&_EVENT_TYPE_());
}

//action = [GLFW_PRESS, GLFW_RELEASE]
void InputHandler::mouseClickCallback(GLFWwindow * window, int button, int action, int mods)
{
	//Send event when mouse is clicked to EventBus
	//EventBus::get().publish(&_EVENT_TYPE_());
}

InputHandler::InputHandler(GLFWwindow* window)
{
	glfwSetKeyCallback(window, this->keyCallback);
	//glfwSetCursorPosCallback(window, this->mouseMoveCallback);
	//glfwSetMouseButtonCallback(window, this->mouseClickCallback);
}


InputHandler::~InputHandler()
{
}
