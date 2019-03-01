#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include "reactphysics3d/collision/CollisionCallback.h"

#include <unordered_map>

class Entity;

class Collision : public rp3d::CollisionCallback
{
public:
	Collision();
	~Collision();

	// Callback function for the testCollision in CollisionHandler
	void notifyContact(const CollisionCallbackInfo &info);

	// Set pointer to entity and bodies pair
	void setEntitesPointer(std::unordered_map<rp3d::CollisionBody*, Entity*>* entites);
private:
	std::unordered_map<rp3d::CollisionBody*, Entity*>* entites;

	/*
		Check if an entity with its components has already been hit.
		Param:
			The entity to be checked.
		Return:
			True if entity is hit, false if not.
	*/
	bool checkIfHit(Entity* e);
};