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


void Level::read(std::string file, EntityManager *entityManager)
{
	//Read file
	//std::ifstream iFile(file);
	//iFile >> jsonFile;

	std::ifstream i("level.json");
	json::json j;
	i >> j;

	//Get amount of arrays to know how many entities to be made
	int size = jsonFile.size();

	for (int i = 0; i < size; i++) 
	{
		Entity entity;
		//Value for "Name" should be string so we dump content
		entity.setName(jsonFile[i]["Name"].dump());
		glm::vec3 position = glm::vec3(jsonFile[i]["X"], jsonFile[i]["X"], jsonFile[i]["X"]);
		std::cout << position.x << " : " << position.y << " : " << position.z << std::endl;
	}
}
