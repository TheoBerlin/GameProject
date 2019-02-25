#pragma once
#include "../Events/Events.h"
#include "Component.h"

/*
	This class is added to all of the targets (drones and whatnot) that should react when a collision to them has happened.
*/

namespace reactphysics3d { class ProxyShapes; }

class TargetMovingCollision : public Component
{
public:
	TargetMovingCollision(Entity * parentEntity, const std::string& tagName = "Collision");
	virtual ~TargetMovingCollision();
	bool getFlag();

	void update(const float& dt);

private:
	bool flag;
	void collide(PlayerCollisionEvent * evnt);
};