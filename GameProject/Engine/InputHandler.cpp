#include "InputHandler.h"

#include "Events/EventBus.h"

//action = [GLFW_PRESS, GLFW_REPEAT, GLFW_RELEASE]
void InputHandler::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	EventBus::get().publish(new KeyEvent(key, action));
}

void InputHandler::mouseMoveCallback(GLFWwindow * window, double xpos, double ypos)
{
	//Send event when mouse is moved to EventBus
}

//action = [GLFW_PRESS, GLFW_RELEASE]
void InputHandler::mouseClickCallback(GLFWwindow * window, int button, int action, int mods)
{
	//Send event when mouse is clicked to EventBus
}

InputHandler::InputHandler(GLFWwindow* window)
{
	glfwSetKeyCallback(window, this->keyCallback);

}


InputHandler::~InputHandler()
{
}
