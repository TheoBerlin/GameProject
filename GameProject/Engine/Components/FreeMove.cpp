#include "FreeMove.h"

#include "../Rendering/Display.h"
#include "../Entity/Entity.h"
#include "glm/gtx/rotate_vector.hpp"
#include "../Events/EventBus.h"
#include "Utils/Logger.h"
#include <Utils/Settings.h>

FreeMove::FreeMove(Entity * parentEntity, const std::string& tagName) : Component(parentEntity, tagName)
{
	EventBus::get().subscribe(this, &FreeMove::moveKeyboard);
	EventBus::get().subscribe(this, &FreeMove::moveMouse);
	EventBus::get().subscribe(this, &FreeMove::clickMouse);

	this->speed = 5.0f;
	this->sensitivity = Settings::get().getMouseSensitivity();
	this->mouseLock = false;
	this->xPos = 0.0;
	this->yPos = 0.0;
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
	Transform * mat = host->getTransform();

	this->dt = dt;

	glm::vec3 newPosition = mat->getPosition();

	// Check keyboard input to calculate new position
	if (this->pressedKeys[GLFW_KEY_W])
		newPosition += this->dt * mat->getForward() * this->speed;
	if (this->pressedKeys[GLFW_KEY_A])
		newPosition += this->dt * -mat->getRight() * this->speed;
	if (this->pressedKeys[GLFW_KEY_D])
		newPosition += this->dt * mat->getRight() * this->speed;
	if (this->pressedKeys[GLFW_KEY_S])
		newPosition += this->dt * -mat->getForward() * this->speed;
	if (this->pressedKeys[GLFW_KEY_SPACE])
		newPosition += this->dt * GLOBAL_UP_VECTOR * this->speed;
	if (this->pressedKeys[GLFW_KEY_LEFT_CONTROL])
		newPosition += this->dt * -GLOBAL_UP_VECTOR * this->speed;

	mat->setPosition(newPosition);

	if (this->mouseLock)
	{
		if (this->xPos != 0.0 || this->yPos != 0.0)
		{
			float yaw = -(float)xPos * this->dt * this->sensitivity;
			float pitch = -(float)yPos * this->dt * this->sensitivity;

			host->getTransform()->rotate(yaw, pitch);

			this->xPos = 0.0;
			this->yPos = 0.0;
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

		if (this->mouseLock)
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void FreeMove::moveMouse(MouseMoveEvent * evnt)
{
	this->xPos = evnt->travelX;
	this->yPos = evnt->travelY;
}

void FreeMove::clickMouse(MouseClickEvent * evnt)
{
}
