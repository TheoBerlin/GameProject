#include "FreeMove.h"

#include "GLFW/glfw3.h"
#include "../Entity/Entity.h"
#include "glm/gtx/rotate_vector.hpp"

FreeMove::FreeMove(const std::string& tagName) : Component(tagName)
{
	EventBus::get().subscribe(this, &FreeMove::moveKeyboard);
	EventBus::get().subscribe(this, &FreeMove::moveMouse);
	EventBus::get().subscribe(this, &FreeMove::clickMouse);

	this->active = false;
}


FreeMove::~FreeMove()
{
}

void FreeMove::init()
{
}

void FreeMove::update(const float & dt)
{
	this->dt = dt;
}

void FreeMove::moveKeyboard(KeyEvent * evnt)
{
	EntityMatrix * mat = getHost()->getMatrix();
	if (evnt->action == GLFW_PRESS)
	{
		switch (evnt->key)
		{
		case GLFW_KEY_W:
			mat->setPosition(this->dt * mat->getForward());

		case GLFW_KEY_A:
			mat->setPosition(this->dt * -mat->getRight());

		case GLFW_KEY_D:
			mat->setPosition(this->dt * mat->getRight());

		case GLFW_KEY_S:
			mat->setPosition(this->dt * -mat->getForward());

		case GLFW_KEY_SPACE:
			mat->setPosition(this->dt * mat->getUp());

		case GLFW_KEY_LEFT_CONTROL:
			mat->setPosition(this->dt * -mat->getUp());
		}
	}
}

void FreeMove::moveMouse(MouseMoveEvent * evnt)
{
	EntityMatrix * mat = getHost()->getMatrix();
	glm::vec3 tempForward = mat->getForward();

	tempForward = glm::rotate(tempForward, this->dt*(float)evnt->xpos, mat->getUp());
	tempForward = glm::rotate(tempForward, this->dt*(float)evnt->ypos, mat->getRight());

	mat->setForward(tempForward);
}

void FreeMove::clickMouse(MouseClickEvent * evnt)
{
	if (evnt->action == GLFW_PRESS && evnt->button == GLFW_MOUSE_BUTTON_1)
	{
		this->active = !this->active;
	}
}
