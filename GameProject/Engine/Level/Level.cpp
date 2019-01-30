#include "Level.h"

void Level::writePosition(int index, glm::vec3 position)
{
	jsonFile[index]["X"] = position.x;
	jsonFile[index]["Y"] = position.y;
	jsonFile[index]["Z"] = position.z;
}

void Level::writeName(int index, std::string name)
{
	jsonFile[index]["Name"] = name;
}


void Level::writeToFile(std::string file, glm::vec3 position, glm::vec3 scale)
{
	std::ofstream oStream(file);
	oStream << std::setw(4) << jsonFile << std::endl;
}


std::string Level::read(std::string file)
{
	std::ifstream i(file);
	i >> jsonFile;
	std::string string = "";

	for (int i = 0; i < jsonFile.size(); i++) {
		string += "Index " + std::to_string(i) + ":\n";
		string += jsonFile[i]["Name"].dump();
		string += "\n";
	}
	
	return string;
}
