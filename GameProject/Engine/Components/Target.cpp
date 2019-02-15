#include "Target.h"

#include "../Entity/Entity.h"
#include <reactphysics3d/reactphysics3d.h>

void Target::setUnactive(PlayerCollisionEvent * evnt)
{
	flag = false;
}

//void Target::setActive(ResetEvent * evnt)
//{
//	flag = true;
//}

Target::Target(Entity * parentEntity, const std::string & tagName) : CollisionComponent(parentEntity, tagName)
{
	flag = true;
	// Set subscribe to resize event to update camera
	EventBus::get().subscribe(this, &Target::setUnactive);
	//EventBus::get().subscribe(this, &Target::setActive);
}

Target::~Target()
{
}

bool Target::getFlag()
{
	return flag;
}

void Target::handleCollision(const rp3d::ProxyShape* self, const rp3d::ProxyShape* other)
{
	this->host->getTransform()->translate({ -1.0, 0.0, -1.0 });
}

void Target::update(const float & dt)
{
}
