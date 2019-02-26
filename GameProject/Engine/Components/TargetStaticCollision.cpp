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
		/*
			Vbo index 2 because the drones meshes has third vbo with colors, 0 mesh index because the drone body is that mesh
		*/
		this->host->getModel()->updateInstancingSpecificData(&glm::vec3(1.0, 0.0, 0.0)[0] ,sizeof(glm::vec3),
								this->host->getRenderingGroupIndex() * sizeof(glm::vec3), 0, 2);
	}
}
