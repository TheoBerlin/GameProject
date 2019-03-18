#include "Collision.h"

#include "Engine/Events/EventBus.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Components/MovingTargetCollision.h"
#include "Engine/Components/StaticTargetCollision.h"

Collision::Collision()
{
	this->entites = nullptr;
}


Collision::~Collision()
{
}

void Collision::notifyContact(const CollisionCallbackInfo & info)
{
	// Get correspondening entity to body
	Entity* entity1 = (*this->entites)[info.body1];
	Entity* entity2 = (*this->entites)[info.body2];
	
	if (!checkIfHit(entity2))
	{
		const rp3d::ProxyShape * shape1 = info.proxyShape1;
		const rp3d::ProxyShape * shape2 = info.proxyShape2;

		EventBus::get().publish(&PlayerCollisionEvent(entity1, entity2, shape1, shape2));
	}
}

void Collision::setEntitesPointer(std::unordered_map<rp3d::CollisionBody*, Entity*>* entites)
{
	this->entites = entites;
}

bool Collision::checkIfHit(Entity * e)
{
	// Get component of hit entity to only send an event if not already hit
	Component* cMov = e->getComponent("MovingTargetCollision");
	Component* cSta = e->getComponent("StaticTargetCollision");

	// Dyanamic cast in if for performance reasons
	if (cMov)
	{
		MovingTargetCollision* mov = dynamic_cast<MovingTargetCollision*>(cMov);
		if (mov->isHit())
			return true;
		return false;
	}
	else if (cSta)
	{
		StaticTargetCollision* sta = dynamic_cast<StaticTargetCollision*>(cSta);
		if (sta->isHit())
			return true;
		return false;
	}
	// If entity doesn't have either Moving or Static collision, it will return false, as we are not checking for that.
	return false;
}
