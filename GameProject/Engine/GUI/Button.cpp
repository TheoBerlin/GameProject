#include "Button.h"

Button::Button() : Panel()
{
	init();
}

Button::~Button()
{
}

bool Button::isActivated() const
{
	return this->active;
}

void Button::mouseClickCallback(MouseClickEvent * evnt)
{
	if (evnt->action == GLFW_PRESS)
		this->pressed = true;
	else if (this->isHovering)
	{
		this->active = true;
	}
}

void Button::mouseMoveCallback(MouseMoveEvent * evnt)
{
	if (evnt->moveX >= this->pos.x && evnt->moveX <= this->pos.x + this->size.x &&
		evnt->moveY >= this->pos.y && evnt->moveY <= this->pos.y + this->size.y)
		this->isHovering = true;
	else
		this->active = false;
}

void Button::init()
{
	EventBus::get().subscribe(this, &Button::mouseClickCallback);
	EventBus::get().subscribe(this, &Button::mouseMoveCallback);
}
