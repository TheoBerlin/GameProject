#pragma once

#include <vector>
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
	void createWalls(Level& level, std::vector<glm::vec3>& points, float height = 5.0f);
	// Create mesh buffer for walls
	void createWallBuffers();

	//Get all wall points of the current level
	std::vector<glm::vec3>& getWallPoints();

private:
	Model* createQuad();
	Model* createPlane();

	std::vector<Entity*> wallEntites;
	std::vector<glm::vec2> scales;
	Model* quad;
	Model* plane;
	std::vector<glm::vec3> wallPoints;
};

