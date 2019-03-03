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

#ifdef ENABLE_COLLISION_BOXES
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
		glm::vec3 offset = glm::vec3(0.f);
		glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
		int index = -1; // The index of the shape this data will affect. If -1, this will effect the one in the same position as the position in the array. 
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
	void constructBoundingBox(Model* modelPtr, Vertex* vertices, unsigned int numVertices, bool makeAABB = false);

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
	std::unordered_map<Model*, std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec3>>> lines;
	std::vector<glm::vec3> lines2;
	std::vector<glm::mat4> matricesLines;
	std::vector<glm::vec3> colorsLines;

	typedef std::pair<glm::vec3, glm::vec3> AABB;
	typedef std::tuple<glm::vec3, glm::vec3, glm::quat> OBB;
	AABB getAABB(Vertex* vertices, unsigned int numVertices, glm::vec3 e1 = { 1.f, 0.f, 0.f }, glm::vec3 e2 = { 0.f, 1.f, 0.f }, glm::vec3 e3 = { 0.f, 0.f, 1.f });
	OBB getOBB(Model* ptr, Vertex* vertices, unsigned int numVertices);
	void constructShape(CollisionShapeDrawingData* data, const glm::vec3& pos, const glm::vec3& size, glm::quat rot = glm::quat(1.f, 0.f, 0.f, 0.f), CATEGORY cat = CATEGORY::NO_COLLISION, const glm::vec3& scale = {1.f, 1.f, 1.f}, const glm::vec3& color = { 1.f, 1.f, 1.f });
	void addCollisionShapeToBody(rp3d::CollisionBody * body, rp3d::CollisionShape* shape, CATEGORY category, const glm::vec3& pos, const glm::quat& rotation, CollisionShapeDrawingData* data = nullptr);

	void addVariedCollisionShapeToBody(unsigned int index, CollisionShapeDrawingData* shape, rp3d::CollisionBody* body, CATEGORY category, const glm::vec3& scale, const glm::vec3& offset, const glm::quat& rotation);
	rp3d::Transform getTransformFromEntity(Entity* entity);

	std::unordered_map<rp3d::CollisionBody*, Entity*> entities;

	unsigned takenBodies;

	typedef std::tuple<float, float, float> MapKey;
	// ---------------------- Hash for the tuple above ----------------------
	struct key_hash : public std::unary_function<MapKey, std::size_t> {
		std::size_t operator()(const MapKey& k) const {
			return ((std::size_t)std::get<0>(k)) ^ ((std::size_t)std::get<1>(k)) ^ ((std::size_t)std::get<2>(k));
		}
	};
	// ------------------ Equal check for the tuple above -------------------
	struct key_equal : public std::binary_function<MapKey, MapKey, bool> {
		bool operator()(const MapKey& v0, const MapKey& v1) const {
			return (std::get<0>(v0) == std::get<0>(v1) &&
				std::get<1>(v0) == std::get<1>(v1) &&
				std::get<2>(v0) == std::get<2>(v1));
		}
	};

	// ------------------------ Data for the map ----------------------------
	struct MapData {
		std::vector<CollisionShapeDrawingData*> shapes;
	};

	// Unordered map which has a key as a tuple of 3 floats (Scale in x, y, z) and data as a vector of CollisionShapeDrawingData (Shape).
	typedef std::unordered_map<const MapKey, MapData, key_hash, key_equal> VarientMap;
	VarientMap varientShapesMap;

	std::unordered_map<Model*,std::vector<CollisionShapeDrawingData*>> shapesMap;

#ifdef ENABLE_COLLISION_BOXES
	std::vector<glm::mat4> matrices;
	std::vector<glm::vec3> colors;

	std::vector<rp3d::ProxyShape*> proxyShapes;

	bool drawCollisionShapes;
	void toggleDrawing(KeyEvent * ev);

	CollisionRenderer cRenderer;
#else
	void addShape(SHAPE shape, CATEGORY cat, const glm::vec3& scale, const glm::vec3& pos = { 0.0, 0.0, 0.0 });
	std::vector<std::vector<CollisionShapeDrawingData*>> shapes;
#endif

};
