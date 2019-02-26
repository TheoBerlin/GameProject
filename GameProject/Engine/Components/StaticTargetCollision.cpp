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
		// Change color on collision of drone entity
		this->host->getModel()->updateInstancingSpecificData(&glm::vec3(1.0, 0.0, 0.0)[0], sizeof(glm::vec3),
			this->host->getRenderingGroupIndex() * sizeof(glm::vec3), 0, 2);
	}
}
