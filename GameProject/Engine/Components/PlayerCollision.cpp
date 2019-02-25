#include "PlayerCollision.h"

#include <reactphysics3d/reactphysics3d.h>
#include "Engine/Events/EventBus.h"

#include "Utils/Logger.h"

PlayerCollision::PlayerCollision(Entity* parentEntity, const std::string& tagName) : Component(parentEntity, tagName)
{
	EventBus::get().subscribe(this, &PlayerCollision::collide);
}


PlayerCollision::~PlayerCollision()
{
}

void PlayerCollision::update(const float & dt)
{
}

void PlayerCollision::collide(PlayerCollisionEvent * evnt)
{
	LOG_INFO("PLAYER COLLIDED");
}