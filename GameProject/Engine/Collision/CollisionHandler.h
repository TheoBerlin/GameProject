#pragma once

#include "Collision.h"

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <unordered_map>
#include <tuple>
#include <utility>

#include "Engine/Config.h"
#include "Engine/Events/EventBus.h"

#include "CollisionInfo.h"
#include "VarientMap.h"

#include "CollisionConfig.h"

#ifdef ENABLE_COLLISION_DEBUG_DRAW
	#include "CollisionRenderer.h"
#endif

// Forward declerations
class Entity;
struct Vertex;
class Model;
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
	// Remove a collision body from the world, this will stop collision checking and destory the pointer
	void removeCollisionBody(rp3d::CollisionBody * body);
	// Remove collision from an entity
	void removeCollisionBody(Entity * entity);

	// Add collision body to an entity with a predefined shape
	void addCollisionToEntity(Entity * entity, CATEGORY cat, bool isPlayer = false, glm::quat rot = glm::quat(1.f, 0.f, 0.f, 0.f));

	struct ShapeData
	{
		CATEGORY category;
		glm::vec3 scale = glm::vec3(1.f);
		glm::vec3 localScale = glm::vec3(1.f);
		glm::vec3 offset = glm::vec3(0.f);
		glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
		int index = -1; // The index of the shape this data will affect. If -1, this will effect the one in the same position as the position in the array. 
		std::string name; // Used like index, insead of -1 it is empty.
	};
	void addCollisionToEntity(Entity* entity, const std::vector<ShapeData>& shapeData, bool isPlayer = false);

	/*
	Construct a bounding box around the model.
	Arguments:
		modelPtr: A pointer to the model which this shape belongs to. This is used when an entity want to use a shape which already exist.
		vertices: A pointer to a list of vertices.
		numVertices: The length of the list of vertices.
		makeAABB: true if the bounding box should be axis-aligned.
	*/
	void constructBoundingBox(Model* modelPtr, Vertex* vertices, unsigned int numVertices, const std::string& name, bool makeAABB = false);

	rp3d::Vector3 toReactVec(const glm::vec3& vec);
	glm::vec3 toGlmVec(const rp3d::Vector3& vec);
	glm::quat toGlmQuat(const rp3d::Quaternion& vec);

#ifdef ENABLE_COLLISION_DEBUG_DRAW
	void updateDrawingData();

	void drawCollisionBoxes();
#endif

private:
	typedef std::pair<glm::vec3, glm::vec3> AABB;
	typedef std::tuple<glm::vec3, glm::vec3, glm::quat> OBB;
	AABB getAABB(Vertex* vertices, unsigned int numVertices, glm::vec3 e1 = { 1.f, 0.f, 0.f }, glm::vec3 e2 = { 0.f, 1.f, 0.f }, glm::vec3 e3 = { 0.f, 0.f, 1.f });
	OBB getOBB(Model* ptr, Vertex* vertices, unsigned int numVertices);
	void constructShape(CollisionShapeDrawingData* data, const glm::vec3& pos, const glm::vec3& size, glm::quat rot = glm::quat(1.f, 0.f, 0.f, 0.f), CATEGORY cat = CATEGORY::NO_COLLISION, const glm::vec3& scale = {1.f, 1.f, 1.f}, const glm::vec3& color = { 1.f, 1.f, 1.f });
	void addCollisionShapeToBody(rp3d::CollisionBody * body, rp3d::CollisionShape* shape, CATEGORY category, const glm::vec3& pos, const glm::quat& rotation, CollisionShapeDrawingData* data = nullptr);
	void addVariedCollisionShapeToBody(unsigned int index, CollisionShapeDrawingData* shape, rp3d::CollisionBody* body, CATEGORY category, const glm::vec3& scale, const glm::vec3& localScale, const glm::vec3& offset, const glm::quat& rotation);
	rp3d::Transform getTransformFromEntity(Entity* entity);

	rp3d::CollisionWorld * world;
	rp3d::CollisionBody * player;
	std::vector<rp3d::CollisionBody*> bodies;
	Collision collision;
	std::unordered_map<rp3d::CollisionBody*, Entity*> entities;
	unsigned takenBodies;
	VarientMap varientShapesMap;
	std::unordered_map<Model*,std::vector<CollisionShapeDrawingData*>> shapesMap;

#ifdef ENABLE_COLLISION_DEBUG_DRAW
	
	#if defined(ENABLE_AXIS_FOR_COLLISION_BOXES)
		std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>> lines;
		std::vector<glm::vec3> linesDivided;
		std::vector<glm::mat4> matricesLines;
		std::vector<glm::vec3> colorsLines;
	#endif

	#if defined(ENABLE_COLLISION_BOXES)
		std::vector<glm::mat4> matrices;
		std::vector<glm::vec3> colors;
	#endif

	void toggleDrawing(KeyEvent * ev);

	bool drawCollisionShapes;
	CollisionRenderer cRenderer;
	std::vector<rp3d::ProxyShape*> proxyShapes;
#endif

};
