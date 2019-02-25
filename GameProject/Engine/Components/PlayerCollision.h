#pragma once

#include "CollisionComponent.h"

class PlayerCollision : public CollisionComponent
{
public:
	PlayerCollision(Entity* parentEntity, const std::string& tagName = "Collision");
	virtual ~PlayerCollision();

	void handleCollision(const reactphysics3d::ProxyShape* self, const reactphysics3d::ProxyShape* other);
	void update(const float& dt);
};

