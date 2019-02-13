#pragma once

#include "Collision.h"

#include <vector>
#include "glm/glm.hpp"

// Enum for the shapes for the CollisionBodies
enum class Shape {
	DRONE,
	BOX,
	ARROW
};

// Forward declerations
class Entity;

class CollisionHandler
{
public:
	CollisionHandler();
	virtual ~CollisionHandler();

	void update(const float& dt);

	// Creates the amount of bodies needed for the level. Done once to have it all after each other in memory.
	void createCollisionBodies(int num);
	// Returns an unused collision body, nullptr if noone is available.
	rp3d::CollisionBody* getUnusedBody();
	// Add collision body to an entity with a predefined shape
	void addCollisionToEntity(Entity * entity, Shape shape);
	// Remove a collision body from the world, this will stop collision checking and destory the pointer
	void removeCollisionBody(rp3d::CollisionBody * body);

	rp3d::Vector3 toReactVec(const glm::vec3& vec);
	glm::vec3 toGlmVec(const rp3d::Vector3& vec);

private:
	rp3d::CollisionWorld * world;
	rp3d::CollisionBody * player;
	std::vector<rp3d::CollisionBody*> bodies;

	Collision collision;

	// Shapes
	rp3d::BoxShape shape = { { 2.0, 2.0, 2.0 } };

	int takenBodies;

};
