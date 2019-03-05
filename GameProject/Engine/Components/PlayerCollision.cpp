#include "PlayerCollision.h"

#include <reactphysics3d/reactphysics3d.h>
#include "Engine/Events/EventBus.h"

// Remove logger when collide does more than log
#include "Utils/Logger.h"

PlayerCollision::PlayerCollision(Entity* parentEntity, const std::string& tagName) : Component(parentEntity, tagName)
{
	EventBus::get().subscribe(this, &PlayerCollision::collide);
}


PlayerCollision::~PlayerCollision()
{
	EventBus::get().unsubscribe(this, &PlayerCollision::collide);
}

void PlayerCollision::update(const float & dt)
{
}

void PlayerCollision::collide(PlayerCollisionEvent * evnt)
{
	LOG_INFO("Collision: %d %d", evnt->shape1->getCollisionCategoryBits(), evnt->shape2->getCollisionCategoryBits());
}
