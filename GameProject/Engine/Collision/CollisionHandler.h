#pragma once

#include "Collision.h"

#include <vector>
#include "glm/glm.hpp"
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
	// Add collision body to an entity with a predefined shape
	//void addCollisionToEntity(Entity * entity, SHAPE shape);
	// Remove a collision body from the world, this will stop collision checking and destory the pointer
	void removeCollisionBody(rp3d::CollisionBody * body);
	// Remove collision from an entity
	void removeCollisionBody(Entity * entity);

	// Add collision body to an entity with a predefined shape
	std::vector<unsigned short*> addCollisionToEntity(Entity * entity, CATEGORY cat, bool isPlayer = false);
	void addShape(Model* modelPtr, Vertex* vertices, unsigned int numVertices);

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
	std::pair<glm::vec3, glm::vec3> getAABB(Vertex* vertices, unsigned int numVertices);
	void constructShape(CollisionShapeDrawingData* data, const glm::vec3& pos, const glm::vec3& size, CATEGORY cat = CATEGORY::NO_COLLISION, const glm::vec3& scale = {1.f, 1.f, 1.f}, const glm::vec3& color = { 1.f, 1.f, 1.f });
	void addCollisionShapeToBody(rp3d::CollisionBody* body, CollisionShapeDrawingData* data, const rp3d::Quaternion& shapeRot);

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

	//void addShape(SHAPE shape, CATEGORY cat, const glm::vec3& scale, const glm::vec3& color = { 0.0f, 1.0f, 0.0f }, const glm::vec3& pos = { 0.0, 0.0, 0.0 });
	//std::vector<std::vector<CollisionShapeDrawingData*>> shapes;
	std::vector<rp3d::ProxyShape*> proxyShapes;

	bool drawCollisionShapes;
	void toggleDrawing(KeyEvent * ev);

	CollisionRenderer cRenderer;
#else
	void addShape(SHAPE shape, CATEGORY cat, const glm::vec3& scale, const glm::vec3& pos = { 0.0, 0.0, 0.0 });
	std::vector<std::vector<CollisionShapeDrawingData*>> shapes;
#endif

};
