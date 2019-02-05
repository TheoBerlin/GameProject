#pragma once
#include "Component.h"
#include "../Events/Events.h"
#include <unordered_map>

class FreeMove : public Component
{
public:
	FreeMove(const std::string& tagName = "FreeMove");
	virtual ~FreeMove();

	void init();
	void update(const float& dt);

private:
	float dt;
	float speed;
	float sensitivity;
	bool mouseLock;
	double xPos, yPos;
	double preXPos, preYPos;
	std::unordered_map<unsigned, bool> pressedKeys;

	void moveKeyboard(KeyEvent * evnt);
	void moveMouse(MouseMoveEvent * evnt);
	void clickMouse(MouseClickEvent * evnt);
};

