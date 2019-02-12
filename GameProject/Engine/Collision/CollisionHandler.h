#pragma once

#include "reactphysics3d/reactphysics3d.h"

class CollisionHandler
{
public:
	CollisionHandler();
	virtual ~CollisionHandler();

private:
	rp3d::CollisionWorld * test;
};

