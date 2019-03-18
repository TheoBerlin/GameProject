#include "StaticTargetCollision.h"

#include "../Entity/Entity.h"
#include <reactphysics3d/reactphysics3d.h>
#include <Engine/Collision/CollisionInfo.h>
#include <Engine/Events/EventBus.h>
#include <Game/Components/DeathAnimation.h>

StaticTargetCollision::StaticTargetCollision(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	this->hit = false;
	this->shape = nullptr;
	EventBus::get().subscribe(this, &StaticTargetCollision::collide);
}

StaticTargetCollision::~StaticTargetCollision()
{
	EventBus::get().unsubscribe(this, &StaticTargetCollision::collide);
}

bool StaticTargetCollision::isHit()
{
	return this->hit;
}

void StaticTargetCollision::enableCollision()
{
	this->hit = false;
}

void StaticTargetCollision::update(const float & dt)
{
	if (this->hit)
	{
		// Look for the proxy shape that contains the collision shape in parameter
		while (shape != nullptr) {
			shape->setCollisionCategoryBits(CATEGORY::NO_COLLISION);

			// Get the next element in the list
			shape = shape->getNext();
		}
	}
}

void StaticTargetCollision::collide(PlayerCollisionEvent * evnt)
{
	if (evnt->entity1 == host || evnt->entity2 == host)
	{
		if (evnt->phase == PlayerCollisionEvent::REPLAY_PHASE)
		{
			DeathAnimation* deathAnimation = dynamic_cast<DeathAnimation*>(this->host->getComponent("DeathAnimation"));
			deathAnimation->play();
		}
		else
		{
			// Change color on collision of drone entity
			this->host->getModel()->updateInstancingSpecificData(&glm::vec3(1.0, 0.0, 0.0)[0], sizeof(glm::vec3),
				this->host->getRenderingGroupIndex() * sizeof(glm::vec3), 0, 2);
		}
		rp3d::CollisionBody* body = evnt->entity2->getCollisionBody();
		this->shape = body->getProxyShapesList();
		this->hit = true;
	}
}