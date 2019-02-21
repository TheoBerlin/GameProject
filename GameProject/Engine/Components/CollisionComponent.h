#pragma once

#include "Component.h"

namespace reactphysics3d { class ProxyShape; }

/*
	This component is a pure virtual. Components which should have handle collision should inherit from this class,
	instead of the normal component.
*/

class CollisionComponent : public Component
{
public:
	CollisionComponent(Entity* parentEntity, const std::string& tagName);
	virtual ~CollisionComponent();

	virtual void handleCollision(const reactphysics3d::ProxyShape* self, const reactphysics3d::ProxyShape* other) = 0;
};

