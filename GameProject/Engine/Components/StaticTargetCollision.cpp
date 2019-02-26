#include "StaticTargetCollision.h"

#include "../Entity/Entity.h"
#include <reactphysics3d/reactphysics3d.h>


StaticTargetCollision::StaticTargetCollision(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	flag = true;
	EventBus::get().subscribe(this, &StaticTargetCollision::collide);
}

StaticTargetCollision::~StaticTargetCollision()
{
	EventBus::get().unsubscribe(this, &StaticTargetCollision::collide);
}

bool StaticTargetCollision::getFlag()
{
	return flag;
}

void StaticTargetCollision::update(const float & dt)
{
}

void StaticTargetCollision::collide(PlayerCollisionEvent * evnt)
{
	if (evnt->entity1 == host || evnt->entity2 == host)
	{
		this->host->getTransform()->translate({ -1.0, 0.0, -1.0 });
	}
}
