#include "Collision.h"

#include "Engine/Events/EventBus.h"
#include "Engine/Entity/Entity.h"

Collision::Collision()
{
}


Collision::~Collision()
{
}

void Collision::notifyContact(const CollisionCallbackInfo & info)
{
	Entity* entity1 = (*this->entites)[info.body1];
	Entity* entity2 = (*this->entites)[info.body2];
	const rp3d::ProxyShape * shape1 = info.proxyShape1;
	const rp3d::ProxyShape * shape2 = info.proxyShape2;

	EventBus::get().publish(&PlayerCollisionEvent(entity1, entity2, shape1, shape2));
}

void Collision::setEntitesPointer(std::unordered_map<rp3d::CollisionBody*, Entity*>* entites)
{
	this->entites = entites;
}
