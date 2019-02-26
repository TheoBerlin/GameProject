#include "MovingTargetCollision.h"

#include "../Entity/Entity.h"
#include "Engine/Events/EventBus.h"
#include <reactphysics3d/reactphysics3d.h>


MovingTargetCollision::MovingTargetCollision(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	flag = true;
	EventBus::get().subscribe(this, &MovingTargetCollision::collide);
}

MovingTargetCollision::~MovingTargetCollision()
{
	EventBus::get().unsubscribe(this, &MovingTargetCollision::collide);
}

bool MovingTargetCollision::getFlag()
{
	return flag;
}

void MovingTargetCollision::update(const float & dt)
{
}

void MovingTargetCollision::collide(PlayerCollisionEvent * evnt)
{
	if (evnt->entity1 == host || evnt->entity2 == host)
	{
		this->host->getTransform()->translate({ 0.0, 2.0, 0.0 });
	}
}
