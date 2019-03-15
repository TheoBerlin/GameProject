#include "Button.h"

#include "../Rendering/Display.h"
#include "../Sound/SoundManager.h"

Button::Button() : Panel()
{
	init();
}

Button::~Button()
{
	EventBus::get().unsubscribe(this, &Button::mouseClickCallback);
	EventBus::get().unsubscribe(this, &Button::mouseMoveCallback);
}

void Button::setHoverTexture(Texture * texture)
{
	this->hoverTexture = texture;
}

void Button::setNormalTexture(Texture * texture)
{
	this->normalTexture = texture;
	toNormalStyle();
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
	toNormalStyle();
}

void Button::setPressedColor(const glm::vec4 & color)
{
	this->pressedColor = color;
}

void Button::setCallback(std::function<void(void)> func)
{
	this->func = func;
}

void Button::removeCallback()
{
	this->func = [](void)->void {};
}

bool Button::hover() const
{
	return this->isHovering;
}

void Button::mouseClickCallback(MouseClickEvent * evnt)
{
	if (this->active)
	{
		if (evnt->action == GLFW_PRESS)
		{
			this->pressed = true;
			if (this->isHovering) {
				if (!this->firstPressed) {
					this->sound.playSound();
					toPressedStyle();
					this->firstPressed = true;
				}
			}
		}
		else if (this->isHovering && evnt->action == GLFW_RELEASE)
		{
			if (this->func)
				this->func();
			toNormalStyle();
			this->firstPressed = false;
		}
	}
}

void Button::mouseMoveCallback(MouseMoveEvent * evnt)
{
	int py = ((int)Display::get().getHeight() - (int)evnt->posY);
	if ((int)evnt->posX >= this->globalPos.x && (int)evnt->posX <= this->globalPos.x + (int)this->size.x &&
		py >= this->globalPos.y && py <= this->globalPos.y + (int)this->size.y && this->active)
	{
		this->isHovering = true;
		toHoverStyle();
	}
	else
	{
		this->isHovering = false;
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

	this->firstPressed = false;
	this->active = true;

	this->sound.loadSound("Game/Assets/sound/button.wav");
	this->sound.setLoopState(false);
	SoundManager::get().addSound(&this->sound, SoundType::SOUND_MISC);
}