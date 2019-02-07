#pragma once

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include "../Entity/EntityManager.h"
#include "../../Utils/Logger.h"
#include "../AssetManagement/ModelLoader.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"
#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"

namespace json = nlohmann;

#define CLASS_NAME "LEVEL_PARSER"

class LevelParser {
private:
	json::json jsonFile;

	/*void writePosition(int index, glm::vec3 position);
	void writeName(int index, std::string name);*/

	void readEntityTargets(EntityManager *entityManager);
	void readEntityBoxes(EntityManager *entityManager);
	void readEntityWalls(EntityManager *entityManager);
	void readEntityPlayer(EntityManager *entityManager);

public:
	//void writeToFile(std::string file, EntityManager *entityManager);
	void readEntites(std::string file, EntityManager *entityManager);

};
