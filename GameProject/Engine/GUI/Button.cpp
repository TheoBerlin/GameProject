#include "Button.h"

Button::Button() : Panel()
{
	init();
}

Button::~Button()
{
}

void Button::setHoverTexture(Texture * texture)
{
	this->hoverTexture = texture;
}

void Button::setNormalTexture(Texture * texture)
{
	this->normalTexture = texture;
}

void Button::setPressedTexture(Texture * texture)
{
	this->pressedTexture = texture;
}

void Button::setHoverColor(const glm::vec4 & color)
{
	this->hoverColor = color;
}

void Button::setNormalColor(const glm::vec4 & color)
{
	this->normalColor = color;
}

void Button::setPressedColor(const glm::vec4 & color)
{
	this->pressedColor = color;
}

bool Button::isActivated() const
{
	return this->active;
}

void Button::setCallback(std::function<void(void)> func)
{
	this->func = func;
}

void Button::mouseClickCallback(MouseClickEvent * evnt)
{
	if (evnt->action == GLFW_PRESS)
	{
		this->pressed = true;
		if(this->isHovering)
			toPressedStyle();
	}
	else if (this->isHovering)
	{
		this->active = true;
		this->func();
		toNormalStyle();
	}
}

void Button::mouseMoveCallback(MouseMoveEvent * evnt)
{
	if (evnt->moveX >= this->pos.x && evnt->moveX <= this->pos.x + this->size.x &&
		evnt->moveY >= this->pos.y && evnt->moveY <= this->pos.y + this->size.y)
	{
		this->isHovering = true;
		toHoverStyle();
	}
	else
	{
		this->isHovering = false; 
		this->active = false;
		toNormalStyle();
	}
}

void Button::toNormalStyle()
{
	if (this->normalTexture != nullptr)
		this->setBackgroundTexture(this->normalTexture);
	else
		this->setColor(this->normalColor);
}

void Button::toHoverStyle()
{
	if (this->hoverTexture != nullptr)
		this->setBackgroundTexture(this->hoverTexture);
	else
		this->setColor(this->hoverColor);
}

void Button::toPressedStyle()
{
	if (this->pressedTexture != nullptr)
		this->setBackgroundTexture(this->pressedTexture);
	else
		this->setColor(this->pressedColor);
}

void Button::init()
{
	EventBus::get().subscribe(this, &Button::mouseClickCallback);
	EventBus::get().subscribe(this, &Button::mouseMoveCallback);

	this->normalColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	this->hoverColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	this->pressedColor = { 0.3f, 0.3f, 0.3f, 1.0f };
}