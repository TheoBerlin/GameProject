#include "Target.h"

void Target::setUnactive(PlayerCollisionEvent * evnt)
{
	flag = false;
}

void Target::setActive(ResetEvent * evnt)
{
	flag = true;
}

Target::Target(Entity * parentEntity, const std::string & tagName) : Component(parentEntity, tagName)
{
	flag = true;
	// Set subscribe to resize event to update camera
	EventBus::get().subscribe(this, &Target::setUnactive);
	EventBus::get().subscribe(this, &Target::setActive);
}

Target::~Target()
{
}

bool Target::getFlag()
{
	return flag;
}
