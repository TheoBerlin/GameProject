#include "Collision.h"

#include "Utils/Logger.h"

Collision::Collision()
{
}


Collision::~Collision()
{
}

void Collision::notifyContact(const CollisionCallbackInfo & collisionCallbackInfo)
{
	LOG_INFO("COLLISION");
}
