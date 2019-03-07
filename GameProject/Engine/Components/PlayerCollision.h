#pragma once

#include "Component.h"
#include "Engine/Events/Events.h"

namespace reactphysics3d { class ProxyShape; }

class PlayerCollision : public Component
{
public:
	PlayerCollision(Entity* parentEntity, const std::string& tagName = "PlayerCollision");
	virtual ~PlayerCollision();

	void update(const float& dt);

private:
	void collide(PlayerCollisionEvent * evnt);
};

