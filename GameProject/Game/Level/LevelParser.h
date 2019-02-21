#pragma once

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <Game/Components/PathTreader.h>
#include <Game/Level/Level.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/AssetManagement/ModelLoader.h>

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

class LevelParser {
private:
	json::json jsonFile;

	/*void writePosition(int index, glm::vec3 position);
	void writeName(int index, std::string name);*/

	void readEntityTargets(Level& level);
	void readEntityBoxes(Level& level);
	void readEntityWalls(Level& level);
	void readEntityFloor(Level& level);

	void readPosition(json::json& file, Entity* entity, glm::vec3& position);
	void readPath(json::json& file, Entity* entity, std::vector<KeyPoint>& path);

	void createCollisionBodies(Level& level);

public:
	//void writeToFile(std::string file, EntityManager *entityManager);
	void readLevel(std::string file, Level& level);

};
