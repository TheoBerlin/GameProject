#ifndef LEVEL_H
#define LEVEL_H
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

class Level {
private:
	//Testing
	std::string workingFile = "level.json";

	void writePosition(int index, glm::vec3 position);

public:
	void writeToFile(glm::vec3 position, glm::vec3 scale);
	std::string read(std::string file);

};

#endif //LEVEL_H