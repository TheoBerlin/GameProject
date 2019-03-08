#pragma once

#include "Panel.h"
#include "../Events/EventBus.h"
#include <functional>

class Button : public Panel
{
public:
	Button();
	virtual ~Button();

	/*
	Set the texture for when the user is hovering the button.
	*/
	void setHoverTexture(Texture* texture);

	/*
	Set the texture for when the user is not hovering or pressing the button.
	*/
	void setNormalTexture(Texture* texture);

	/*
	Set the texture for when the user is pressing the button.
	*/
	void setPressedTexture(Texture* texture);

	/*
	Set the color for when the user is hovering the button.
	*/
	void setHoverColor(const glm::vec4& color);

	/*
	Set the color for when the user is not hovering or pressing the button.
	*/
	void setNormalColor(const glm::vec4& color);

	/*
	Set the color for when the user is pressing the button.
	*/
	void setPressedColor(const glm::vec4& color);

	/*
	Set the callback function. This will be called when the button is activated.
	*/
	void setCallback(std::function<void(void)> func);

	/*
	Remove the callback function.
	*/
	void removeCallback();

	/*
	Check if the button is pressed
	*/
	bool hover() const;

private:
	/*
	A callback to be called when the MB1 is pressed or released.
	*/
	void mouseClickCallback(MouseClickEvent* evnt);

	/*
	A callback to be called when the mouse is moving.
	*/
	void mouseMoveCallback(MouseMoveEvent* evnt);

	/*
	Will set the texture to the normal texture if exist else to the normal color.
	*/
	void toNormalStyle();

	/*
	Will set the texture to the hover texture if exist else to the hover color.
	*/
	void toHoverStyle();

	/*
	Will set the texture to the pressed texture if exist else to the pressed color.
	*/
	void toPressedStyle();

	/*
	Initialize the button.
	*/
	void init();

	/*
	Set if the button is active.
	*/
	void setActive(bool active) override;

	/*
	Get if the button is active.
	*/
	bool isActive() const override;

private:
	bool active;
	bool isHovering;
	bool pressed;

	Texture* normalTexture;
	Texture* hoverTexture;
	Texture* pressedTexture;
	glm::vec4 normalColor;
	glm::vec4 hoverColor;
	glm::vec4 pressedColor;

	std::function<void(void)> func;
};