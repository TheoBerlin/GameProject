#include "FreeMove.h"

#include "../Rendering/Display.h"
#include "../Entity/Entity.h"
#include "glm/gtx/rotate_vector.hpp"
#include "../Events/EventBus.h"
#include <Utils/Settings.h>

FreeMove::FreeMove(Entity * parentEntity, const std::string& tagName) : Component(parentEntity, tagName)
{
	EventBus::get().subscribe(this, &FreeMove::moveKeyboard);

	this->speed = 5.0f;
	this->sensitivity = Settings::get().getMouseSensitivity();
	this->enableMouse();
	this->xPos = 0;
	this->yPos = 0;

	// Calculate pitch
	glm::vec3 forward = host->getTransform()->getForward();
	glm::vec3 horizontalForward = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

	this->currentPitch = std::acosf(glm::dot(horizontalForward, forward));

	// Determine pitch sign
	this->currentPitch = (forward.y > 0.0f) ? currentPitch : -currentPitch;
}

FreeMove::~FreeMove()
{
	EventBus::get().unsubscribe(this, &FreeMove::moveKeyboard);
	EventBus::get().unsubscribe(this, &FreeMove::moveMouse);
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

	if (this->xPos != 0 || this->yPos != 0)
	{
		float yaw = -(float)xPos * this->dt * this->sensitivity;
		float pitch = -(float)yPos * this->dt * this->sensitivity;

		// Limit pitch
		float newPitch = currentPitch + pitch;

		if (newPitch > maxPitch) {
			pitch = maxPitch - currentPitch;
		} else if (newPitch < -maxPitch) {
			pitch = -maxPitch - currentPitch;
		}

		currentPitch += pitch;

		host->getTransform()->rotate(yaw, pitch);

		this->xPos = 0;
		this->yPos = 0;
	}
}

void FreeMove::setSpeed(const float speed)
{
	this->speed = speed;
}

float FreeMove::getSpeed() const
{
	return speed;
}

void FreeMove::toggleMouse()
{
	if (this->mouseEnabled) {
		disableMouse();
	} else {
		enableMouse();
	}
}

void FreeMove::enableMouse()
{
	if (!this->mouseEnabled) {
		EventBus::get().subscribe(this, &FreeMove::moveMouse);

		this->mouseEnabled = true;
	}
}

void FreeMove::disableMouse()
{
	if (this->mouseEnabled) {
		EventBus::get().unsubscribe(this, &FreeMove::moveMouse);

		this->mouseEnabled = false;
	}
}

bool FreeMove::mouseIsEnabled() const
{
	return this->mouseEnabled;
}

void FreeMove::moveKeyboard(KeyEvent * evnt)
{
	if (evnt->action == GLFW_PRESS)
		this->pressedKeys[evnt->key] = true;
	else if (evnt->action == GLFW_RELEASE)
		this->pressedKeys[evnt->key] = false;
}

void FreeMove::moveMouse(MouseMoveEvent * evnt)
{
	this->xPos = evnt->deltaX;
	this->yPos = evnt->deltaY;
}
