#include "Level.h"

void Level::writePosition(int index, glm::vec3 position)
{
	json::json j;
	std::ifstream i(workingFile);
	i >> j;

	j[index]["X"] = position.x;
	j[index]["Y"] = position.y;
	j[index]["Z"] = position.z;

}


void Level::writeToFile(glm::vec3 position, glm::vec3 scale)
{
	json::json j;
	std::ifstream i(workingFile);
	i >> j;

	//{position.x, position.y, position.z }
	j.push_back({"X", position.x});

	std::ofstream file(workingFile);
	file << std::setw(4) << j << std::endl;
}


std::string Level::read(std::string file)
{
	json::json j;
	std::ifstream i(file);
	i >> j;

	

	return std::to_string(j.size());
}
