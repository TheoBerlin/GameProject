#pragma once
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

class Level {
private:
	json::json jsonFile;

	void writePosition(int index, glm::vec3 position);
	void writeName(int index, std::string name);

public:
	void writeToFile(std::string file, glm::vec3 position, glm::vec3 scale);
	std::string read(std::string file);
};

