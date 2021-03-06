#pragma once
#include <Game/GameLogic/GameEvents.h>
#include "Component.h"

/*
	This class is added to all of the targets (drones and whatnot) that should react when a collision to them has happened.
*/

namespace reactphysics3d { class ProxyShape; }

class StaticTargetCollision : public Component
{
public:
	StaticTargetCollision(Entity * parentEntity, const std::string& tagName = "StaticTargetCollision");
	virtual ~StaticTargetCollision();

	// Returns the current flag status
	bool isHit();
	// Reset the flag to false
	void enableCollision();

	void update(const float& dt);

private:
	bool hit;
	void collide(PlayerCollisionEvent * evnt);

	reactphysics3d::ProxyShape* shape;
};