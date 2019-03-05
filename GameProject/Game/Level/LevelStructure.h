#pragma once

#include <vector>
#include "glm/glm.hpp"

class Model;
struct Level;

class LevelStructure
{
public:
	LevelStructure();
	virtual ~LevelStructure();

	// Create walls
	void createWalls(Level& level, std::vector<glm::vec3>& points, float height = 5.0f);

	std::vector<glm::vec3>& getWallPoints();

private:
	Model* createQuad();

	Model* quad;
	std::vector<glm::vec3> wallPoints;
};

