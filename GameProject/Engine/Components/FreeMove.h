#pragma once
#include "Component.h"
#include "../Events/Events.h"

class FreeMove : public Component
{
public:
	FreeMove(const std::string& tagName = "FreeMove");
	virtual ~FreeMove();

	void init();
	void update(const float& dt);

private:
	float dt;
	bool active;
	float speed;

	void moveKeyboard(KeyEvent * evnt);
	void moveMouse(MouseMoveEvent * evnt);
	void clickMouse(MouseClickEvent * evnt);
};

