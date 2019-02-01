#pragma once
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include "../Entity/EntityManager.h"
#include "../../Utils/Logger.h"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

//Startning line of error messages
#define CLASS_NAME "LEVEL"

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
	//Takes JSON file path and pointer of entityManager. OBS! make sure level has correct path relative main.cpp location.
	void writeToFile(std::string file, EntityManager *entityManager);
	//Takes JSON file path and pointer of entityManager. OBS! make sure level has correct path relative main.cpp location.
	void read(std::string file, EntityManager *entityManager);
};

