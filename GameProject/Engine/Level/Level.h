#pragma once
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include "../Entity/EntityManager.h"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

/*
LEVEL JSON FILE STRUCTURE
{
	{
		Name: "name",
		X: x,
		Y: y,
		Z: y
	}
	{
		Name: "name1",
		X: x,
		Y: y,
		Z: y
	}
}

*/

class Level {
private:
	json::json jsonFile;

	void writePosition(int index, glm::vec3 position);
	void writeName(int index, std::string name);

public:
	void writeToFile(std::string file, glm::vec3 position, glm::vec3 scale);
	void read(std::string file, EntityManager *entityManager);
};

