#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include "reactphysics3d/collision/CollisionCallback.h"

class Collision : public rp3d::CollisionCallback
{
public:
	Collision();
	~Collision();

	void notifyContact(const CollisionCallbackInfo &collisionCallbackInfo);

private:

};