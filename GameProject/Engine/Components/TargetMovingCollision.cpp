#include "TargetMovingCollision.h"

#include "../Entity/Entity.h"
#include "Engine/Events/EventBus.h"
#include <reactphysics3d/reactphysics3d.h>


TargetMovingCollision::TargetMovingCollision(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	flag = true;
	EventBus::get().subscribe(this, &TargetMovingCollision::collide);
}

TargetMovingCollision::~TargetMovingCollision()
{
	EventBus::get().unsubscribe(this, &TargetMovingCollision::collide);
}

bool TargetMovingCollision::getFlag()
{
	return flag;
}

void TargetMovingCollision::update(const float & dt)
{
}

void TargetMovingCollision::collide(PlayerCollisionEvent * evnt)
{
	if (evnt->entity1 == host || evnt->entity2 == host)
	{
		this->host->getTransform()->translate({ 0.0, 2.0, 0.0 });
	}
}
