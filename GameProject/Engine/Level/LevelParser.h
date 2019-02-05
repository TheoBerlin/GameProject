#pragma once

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include "../Entity/EntityManager.h"
#include "../../Utils/Logger.h"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

#define CLASS_NAME "LEVEL_PARSER"

class LevelParser {
private:
	json::json jsonFile;

	void writePosition(int index, glm::vec3 position);
	void writeName(int index, std::string name);

public:
	void writeToFile(std::string file, EntityManager *entityManager);
	void read(std::string file, EntityManager *entityManager);
};
