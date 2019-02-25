#include "TargetStaticCollision.h"

#include "../Entity/Entity.h"
#include <reactphysics3d/reactphysics3d.h>


TargetStaticCollision::TargetStaticCollision(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	flag = true;
	EventBus::get().subscribe(this, &TargetStaticCollision::collide);
}

TargetStaticCollision::~TargetStaticCollision()
{
}

bool TargetStaticCollision::getFlag()
{
	return flag;
}

void TargetStaticCollision::update(const float & dt)
{
}

void TargetStaticCollision::collide(PlayerCollisionEvent * evnt)
{
	if (evnt->entity1 == host || evnt->entity2 == host)
	{
		this->host->getTransform()->translate({ -1.0, 0.0, -1.0 });
	}
}
