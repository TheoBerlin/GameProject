#pragma once
#include "CollisionComponent.h"
#include "../Events/EventBus.h"

/*
	This class is added to all of the targets (drones and whatnot) that should react when a collision to them has happened.
*/

class TargetCollision : public CollisionComponent
{
public:
	TargetCollision(Entity * parentEntity, const std::string& tagName = "Collision");
	virtual ~TargetCollision();
	bool getFlag();

	// This function handles the collision for targets which has this component.
	void handleCollision(const reactphysics3d::ProxyShape* self, const reactphysics3d::ProxyShape* other);
	void update(const float& dt);

private:
	bool flag;
	void setUnactive(PlayerCollisionEvent * evnt);
	//void setActive(ResetEvent * evnt);
};