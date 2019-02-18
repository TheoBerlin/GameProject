#pragma once

#include "Panel.h"
#include "../Events/EventBus.h"

class Button : public Panel
{
public:
	Button();
	virtual ~Button();

	bool isActivated() const;

private:
	void mouseClickCallback(MouseClickEvent* evnt);
	void mouseMoveCallback(MouseMoveEvent* evnt);
	void init();

	bool active;
	bool isHovering;
	bool pressed;
};
