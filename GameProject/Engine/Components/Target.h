#pragma once
#include "Component.h"
#include "../Events/EventBus.h"

class Target : public Component
{
private:
	bool flag;
	void setUnactive(CollisionEvent * evnt);
	void setActive(ResetEvent * evnt);
public:
	Target(Entity * parentEntity, const std::string& tagName = "Target");
	virtual ~Target();
	bool getFlag();
};