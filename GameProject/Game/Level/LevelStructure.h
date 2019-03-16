#pragma once

#include <vector>
#include <Utils/Utils.h>
#include "glm/glm.hpp"

class Model;
class Entity;
class Transform;
class CollisionHandler;
struct Level;

class LevelStructure
{
public:
	LevelStructure();
	virtual ~LevelStructure();

	// Create walls
	void createWalls(Level& level, std::vector<std::vector<glm::vec3>>& points);
	// Create wall group
	void createWallGroup(Level& level, std::vector<glm::vec3>& points, bool invertNormals = false);

	void addPoint(Level& level, int wallGroupIndex, unsigned pointIndex);
	void editPoint(Level& level, int wallGroupIndex, int point, glm::vec3 newPoint);
	void addWall(Level& level);

	glm::vec2 calcWallTransform(const glm::vec3& p1, const glm::vec3& p2, Transform* trans);

	void addWallCollision(Entity * entity, CollisionHandler* ch);

	void setTexture(std::string fileName);
	std::string getTexture() const;

	void removePoint(Level& level, int wallGroupIndex, unsigned pointIndex);

	// Update buffers
	void updateBuffers();

	// Get all wall points of the current level
	std::vector<glm::vec3>& getWallPoints();
	// Get all wall groups index
	std::vector<int>& getWallGroupsIndex();

	void setWallHeight(float height);
	float getWallHeight() const;

	// Get a AABB around the outer walls which is oriented to the axis: e1, e2, e3
	Utils::AABB createBoundingBox(const glm::vec3& e1, const glm::vec3& e2, const glm::vec3& e3);

private:
	// Create quad model for walls
	Model* createQuad();
	// Create plane model for infinity plane
	Model* createInfPlane();
	// Create plane model for roof
	Model* createRoofPlane();
	// Create mesh buffer for walls
	void createWallBuffers();

	// Create infinity plane
	void createInfinityPlane(Level& level);
	// Create roof plane
	void createRoof(Level& level);
	// Check if the points are expressed in clockwise or anticlockwise order
	bool isClockwise(std::vector<glm::vec3>& points);

	std::string textureFile;
	std::vector<Entity*> wallEntites;
	std::vector<glm::vec2> scales;
	Model* quad;
	Model* infPlane;
	Model* roofPlane;
	std::vector<glm::vec3> wallPoints;
	std::vector<int> wallGroupsIndex;
	float height;
	int spawnedPoints;
};