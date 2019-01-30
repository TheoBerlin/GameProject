#include "Level.h"

void Level::writePosition(int index, glm::vec3 position)
{
	j[index]["X"] = position.x;
	j[index]["Y"] = position.y;
	j[index]["Z"] = position.z;
}

void Level::writeName(int index, std::string name)
{
	j[index]["Name"] = name;
}


void Level::writeToFile(glm::vec3 position, glm::vec3 scale)
{
	json::json j;
	std::ifstream i(workingFile);
	i >> j;

	std::ofstream file(workingFile);
	file << std::setw(4) << j << std::endl;
}


std::string Level::read(std::string file)
{
	std::ifstream i(file);
	i >> j;
	std::string string = "";

	for (int i = 0; i < j.size(); i++) {
		string += "Index " + std::to_string(i) + ":\n";
		string += j[i]["Name"].dump();
		string += "\n";
	}
	
	return string;
}
