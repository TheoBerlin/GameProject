#pragma once

#include <vector>
#include <Utils/Utils.h>
#include "glm/glm.hpp"

class Model;
class Entity;
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

	// Get all wall points of the current level
	std::vector<glm::vec3>& getWallPoints();
	// Get all wall groups index
	std::vector<int>& getWallGroupsIndex();

	// Get a AABB around the outer walls which is oriented to the axis: e1, e2, e3
	Utils::AABB createBoundingBox(const glm::vec3& e1, const glm::vec3& e2, const glm::vec3& e3);

private:
	// Create quad model for walls
	Model* createQuad();
	// Create plane model for infinity plane
	Model* createPlane();
	// Create mesh buffer for walls
	void createWallBuffers();
	// Create infinity plane
	void createInfinityPlane(Level& level);
	// Check if the points are expressed in clockwise or anticlockwise order
	bool isClockwise(std::vector<glm::vec3>& points); 

	std::vector<Entity*> wallEntites;
	std::vector<glm::vec2> scales;
	Model* quad;
	Model* plane;
	std::vector<glm::vec3> wallPoints;
	std::vector<int> wallGroupsIndex;
	float height;
};

