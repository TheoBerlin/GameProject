#include "MovingTargetCollision.h"

#include "../Entity/Entity.h"
#include "Engine/Events/EventBus.h"
#include <reactphysics3d/reactphysics3d.h>
#include <Engine/Collision/CollisionInfo.h>


MovingTargetCollision::MovingTargetCollision(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	this->flag = false;
	EventBus::get().subscribe(this, &MovingTargetCollision::collide);
}

MovingTargetCollision::~MovingTargetCollision()
{
	EventBus::get().unsubscribe(this, &MovingTargetCollision::collide);
}

bool MovingTargetCollision::getFlag()
{
	return this->flag;
}

void MovingTargetCollision::resetFlag()
{
	this->flag = false;
}

void MovingTargetCollision::update(const float & dt)
{
	if (this->flag)
	{
		// Look for the proxy shape that contains the collision shape in parameter
		while (shape != nullptr) {
			shape->setCollisionCategoryBits(CATEGORY::NO_COLLISION);

			// Get the next element in the list
			shape = shape->getNext();
		}
	}
}

void MovingTargetCollision::collide(PlayerCollisionEvent * evnt)
{
	if (evnt->entity1 == host || evnt->entity2 == host)
	{
		this->host->getTransform()->translate({ 0.0, 2.0, 0.0 });

		rp3d::CollisionBody* body = evnt->entity2->getCollisionBody();

		shape = body->getProxyShapesList();

		this->flag = true;
	}
}
