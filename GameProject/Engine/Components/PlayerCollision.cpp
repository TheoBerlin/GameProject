#include "PlayerCollision.h"

#include <reactphysics3d/reactphysics3d.h>
#include "Utils/Logger.h"

PlayerCollision::PlayerCollision(Entity* parentEntity, const std::string& tagName) : CollisionComponent(parentEntity, tagName)
{
}


PlayerCollision::~PlayerCollision()
{
}

void PlayerCollision::handleCollision(const rp3d::ProxyShape* self, const rp3d::ProxyShape* other)
{
	LOG_INFO("PLAYER COLLIDED");
}

void PlayerCollision::update(const float & dt)
{
}
