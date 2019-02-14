#include "Collision.h"

#include "Utils/Logger.h"
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

	//LOG_INFO("Collision: %s, %s", entity1->getName().c_str(), entity2->getName().c_str());
	EventBus::get().publish(&PlayerCollisionEvent(entity1, entity2));
}

void Collision::setEntitesPointer(std::unordered_map<rp3d::CollisionBody*, Entity*>* entites)
{
	this->entites = entites;
}
