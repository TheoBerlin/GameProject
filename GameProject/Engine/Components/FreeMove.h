#pragma once
#include "Component.h"
#include "../Events/Events.h"
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>

class FreeMove : public Component
{
public:
	FreeMove(Entity * parentEntity, const std::string& tagName = "FreeMove");
	~FreeMove();

	void update(const float& dt);

	void enableMouse();
	void disableMouse();

private:
	float dt;
	float speed;
	float sensitivity;
	float currentPitch;
	float maxPitch = glm::half_pi<float>() - 0.01f;
	bool mouseEnabled;
	int xPos, yPos;
	std::unordered_map<unsigned, bool> pressedKeys;

	void moveKeyboard(KeyEvent * evnt);
	void moveMouse(MouseMoveEvent * evnt);
};
