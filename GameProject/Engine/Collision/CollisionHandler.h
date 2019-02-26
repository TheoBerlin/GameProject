#pragma once

#include "Collision.h"

#include <vector>
#include "glm/glm.hpp"
#include <unordered_map>

#include "Engine/Config.h"
#include "Engine/Events/EventBus.h"

#ifdef ENABLE_COLLISION_BOXES
	#include "CollisionRenderer.h"


	struct CollisionShapeDrawingData {
		glm::vec3 color;
		glm::vec3 scale;
		glm::vec3 pos;
		unsigned short category;
		rp3d::CollisionShape * shape;
	};
#else
	struct CollisionShapeDrawingData {
		glm::vec3 pos;
		rp3d::CollisionShape * shape;
	};
#endif

// Enum for the shapes for the CollisionBodies
enum class SHAPE {
	DRONE = 0,
	BOX = 1,
	ARROW = 2,
	SIZE
};

enum CATEGORY
{
	NO_COLLISION = 0x0000,
	ARROW = 0x0001,
	STATIC = 0x0002,
	DRONE_BODY = 0x0004,
	DRONE_EYE = 0x0008
};

// Forward declerations
class Entity;

class CollisionHandler
{
public:
	CollisionHandler();
	virtual ~CollisionHandler();

	void checkCollision();

	// Creates the amount of bodies needed for the level. Done once to have it all after each other in memory.
	void createCollisionBodies(unsigned num);
	// Returns an unused collision body, nullptr if noone is available.
	rp3d::CollisionBody* getUnusedBody();
	// Add collision body to an entity with a predefined shape
	void addCollisionToEntity(Entity * entity, SHAPE shape);
	// Remove a collision body from the world, this will stop collision checking and destory the pointer
	void removeCollisionBody(rp3d::CollisionBody * body);
	// Remove collision from an entity
	void removeCollisionBody(Entity * entity);

	rp3d::Vector3 toReactVec(const glm::vec3& vec);
	glm::vec3 toGlmVec(const rp3d::Vector3& vec);
	glm::quat toGlmQuat(const rp3d::Quaternion& vec);

#ifdef ENABLE_COLLISION_BOXES
	void updateDrawingData();

	void drawCollisionBoxes();
#endif

private:
	rp3d::CollisionWorld * world;
	rp3d::CollisionBody * player;
	std::vector<rp3d::CollisionBody*> bodies;
	
	Collision collision;

	// Creates all the shapes which will be used in the scene
	void createShapes();
	
	std::unordered_map<rp3d::CollisionBody*, Entity*> entities;

	unsigned takenBodies;

#ifdef ENABLE_COLLISION_BOXES
	std::vector<glm::mat4> matrices;
	std::vector<glm::vec3> colors;

	void addShape(SHAPE shape, CATEGORY cat, const glm::vec3& scale, const glm::vec3& color = { 0.0f, 1.0f, 0.0f }, const glm::vec3& pos = { 0.0, 0.0, 0.0 });
	std::vector<std::vector<CollisionShapeDrawingData*>> shapes;
	std::vector<rp3d::ProxyShape*> proxyShapes;

	bool drawCollisionShapes;
	void toggleDrawing(KeyEvent * ev);

	CollisionRenderer cRenderer;
#else
	void addShape(SHAPE shape, CATEGORY cat, const glm::vec3& scale, const glm::vec3& pos = { 0.0, 0.0, 0.0 });
	std::vector<std::vector<CollisionShapeDrawingData*>> shapes;
#endif

};
