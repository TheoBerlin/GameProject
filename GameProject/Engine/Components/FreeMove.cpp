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

	this->active = false;
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
	this->dt = dt;
}

void FreeMove::moveKeyboard(KeyEvent * evnt)
{
	EntityMatrix * mat = getHost()->getMatrix();
	//if (evnt->action == GLFW_PRESS)
	//{
	static const float speed = 100.0f;
	
	switch (evnt->key)
	{
	case GLFW_KEY_W:
		mat->setPosition(mat->getPosition() + this->dt * mat->getForward() * speed);
		break;
	case GLFW_KEY_A:
		mat->setPosition(mat->getPosition() + this->dt * -mat->getRight() * speed);
		break;
	case GLFW_KEY_D:
		mat->setPosition(mat->getPosition() + this->dt * mat->getRight() * speed);
		break;
	case GLFW_KEY_S:
		mat->setPosition(mat->getPosition() + this->dt * -mat->getForward() * speed);
		break;
	case GLFW_KEY_SPACE:
		mat->setPosition(mat->getPosition() + this->dt * mat->getUp() * speed);
		break;
	case GLFW_KEY_LEFT_CONTROL:
		mat->setPosition(mat->getPosition() + this->dt * -mat->getUp() * speed);
		break;
	case GLFW_KEY_LEFT:
		{
		glm::vec3 tempForward = mat->getForward();
		tempForward = glm::rotate(tempForward, this->dt*speed, mat->getUp());
		mat->setForward(tempForward);
		}
		break;
	case GLFW_KEY_RIGHT:
	{
		glm::vec3 tempForward = mat->getForward();
		tempForward = glm::rotate(tempForward, -this->dt*speed, mat->getUp());
		mat->setForward(tempForward);
	}
		break;
	case GLFW_KEY_UP:
	{
		glm::vec3 tempForward = mat->getForward();
		tempForward = glm::rotate(tempForward, this->dt*speed, mat->getRight());
		mat->setForward(tempForward);
	}
		break;
	case GLFW_KEY_DOWN:
	{
		glm::vec3 tempForward = mat->getForward();
		tempForward = glm::rotate(tempForward, -this->dt*speed, mat->getRight());
		mat->setForward(tempForward);
	}
		break;
	}
	
	//}
}

void FreeMove::moveMouse(MouseMoveEvent * evnt)
{
	/*
	EntityMatrix * mat = getHost()->getMatrix();
	glm::vec3 tempForward = mat->getForward();

	tempForward = glm::rotate(tempForward, this->dt*(float)evnt->xpos, mat->getUp());
	tempForward = glm::rotate(tempForward, this->dt*(float)evnt->ypos, mat->getRight());

	mat->setForward(tempForward);*/
}

void FreeMove::clickMouse(MouseClickEvent * evnt)
{
	if (evnt->action == GLFW_PRESS)
	{
		EntityMatrix * mat = getHost()->getMatrix();
		this->active = !this->active;
	}
}
