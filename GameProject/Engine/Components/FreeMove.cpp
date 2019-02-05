#include "FreeMove.h"

#include "../Rendering/Display.h"
#include "../Entity/Entity.h"
#include "glm/gtx/rotate_vector.hpp"
#include "../Events/EventBus.h"
#include "Utils/Logger.h"

FreeMove::FreeMove(const std::string& tagName) : Component(tagName)
{
	EventBus::get().subscribe(this, &FreeMove::moveKeyboard);
	EventBus::get().subscribe(this, &FreeMove::moveMouse);
	EventBus::get().subscribe(this, &FreeMove::clickMouse);

	this->speed = 100.0f;
	this->sensitivity = 10.0f;
	this->mouseLock = false;
	this->xPos = 0.0;
	this->yPos = 0.0;
	this->preXPos = 0.0;
	this->preYPos = 0.0;
}


FreeMove::~FreeMove()
{
	EventBus::get().unsubscribe(this, &FreeMove::moveKeyboard);
	EventBus::get().unsubscribe(this, &FreeMove::moveMouse);
	EventBus::get().unsubscribe(this, &FreeMove::clickMouse);
}

void FreeMove::init()
{
}

void FreeMove::update(const float & dt)
{
	EntityMatrix * mat = getHost()->getMatrix();
	this->dt = dt;

	// Keys that need constant polling
	if (this->pressedKeys[GLFW_KEY_W])
		mat->setPosition(mat->getPosition() + this->dt * mat->getForward() * this->speed);
	if (this->pressedKeys[GLFW_KEY_A])
		mat->setPosition(mat->getPosition() + this->dt * -mat->getRight() * this->speed);
	if (this->pressedKeys[GLFW_KEY_D])
		mat->setPosition(mat->getPosition() + this->dt * mat->getRight() * this->speed);
	if (this->pressedKeys[GLFW_KEY_S])
		mat->setPosition(mat->getPosition() + this->dt * -mat->getForward() * this->speed);
	if (this->pressedKeys[GLFW_KEY_SPACE])
		mat->setPosition(mat->getPosition() + this->dt * mat->getUp() * this->speed);
	if (this->pressedKeys[GLFW_KEY_LEFT_CONTROL])
		mat->setPosition(mat->getPosition() + this->dt * -mat->getUp() * this->speed);

	if (this->mouseLock)
	{
		if (this->xPos != 0.0 || this->yPos != 0.0)
		{
			EntityMatrix * mat = getHost()->getMatrix();
			glm::vec3 oldForward = mat->getForward();

			oldForward = glm::rotate(oldForward, -(float)xPos * this->dt * this->sensitivity, mat->getUp());
			oldForward = glm::rotate(oldForward, -(float)yPos * this->dt * this->sensitivity, mat->getRight());

			this->xPos = 0.0;
			this->yPos = 0.0;

			getHost()->getMatrix()->setForward(oldForward);
		}
	}
}

void FreeMove::moveKeyboard(KeyEvent * evnt)
{
	if (evnt->action == GLFW_PRESS)
		this->pressedKeys[evnt->key] = true;
	else if (evnt->action == GLFW_RELEASE)
		this->pressedKeys[evnt->key] = false;

	// Toggle keys
	if (evnt->key == GLFW_KEY_C && evnt->action == GLFW_PRESS)
	{
		this->mouseLock = !this->mouseLock;
		//glfwSetCursorPos(Display::get().getWindowPtr(), Display::get().getWidth() / 2, Display::get().getHeight() / 2);
		
		if (this->mouseLock)
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void FreeMove::moveMouse(MouseMoveEvent * evnt)
{
	this->xPos = evnt->xpos - this->preXPos;
	this->yPos = evnt->ypos - this->preYPos;
	this->preXPos = evnt->xpos;
	this->preYPos = evnt->ypos;

	//this->xPos -= Display::get().getWidth() / 2;
	//this->yPos -= Display::get().getHeight() / 2;

	LOG_PRINT("XPOS: %f, YPOS: %f", this->xPos, this->yPos);
}

void FreeMove::clickMouse(MouseClickEvent * evnt)
{
}
