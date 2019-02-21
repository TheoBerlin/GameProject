#include "TargetCollision.h"

#include "../Entity/Entity.h"
#include <reactphysics3d/reactphysics3d.h>

void TargetCollision::setUnactive(PlayerCollisionEvent * evnt)
{
	flag = false;
}

//void Target::setActive(ResetEvent * evnt)
//{
//	flag = true;
//}

TargetCollision::TargetCollision(Entity * parentEntity, const std::string & tagName) : CollisionComponent(parentEntity, tagName)
{
	flag = true;
	// Set subscribe to resize event to update camera
	EventBus::get().subscribe(this, &TargetCollision::setUnactive);
	//EventBus::get().subscribe(this, &Target::setActive);
}

TargetCollision::~TargetCollision()
{
}

bool TargetCollision::getFlag()
{
	return flag;
}

void TargetCollision::handleCollision(const rp3d::ProxyShape* self, const rp3d::ProxyShape* other)
{
	this->host->getTransform()->translate({ -1.0, 0.0, -1.0 });
}

void TargetCollision::update(const float & dt)
{
}
