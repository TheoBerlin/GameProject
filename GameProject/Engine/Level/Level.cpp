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
	writeName(0, "Test");
	//writePosition(0, position);

	std::ofstream oStream(file);
	oStream << std::setw(4) << jsonFile << std::endl;
}


void Level::read(std::string file, EntityManager *entityManager)
{
	std::ifstream iFile(file);
	iFile >> jsonFile;

	//Get amount of arrays to know how many entities to be made
	int size = jsonFile.size();
	bool readError = false;

	for (int i = 0; i < size; i++) 
	{
		Entity entity;
		glm::vec3 position;
		//Value for "Name" should be string so we dump content
		entity.setName(jsonFile[i]["Name"].dump());

		std::string axis = "X";
		for (int j = 0; j < 3; j++) {
			if (!jsonFile[i][axis].empty()) {
				try {
					position[j] = jsonFile[i][axis];
				}
				catch (const std::exception& error) {
					LOG_ERROR(error.what());
					readError = true;
					break;
				}
			}
			else {
				position[j] = 0.0;
				LOG_WARNING("LEVEL: Did not find 'X' value defaulting to 0");
			}
			axis[0] += 1;
		}
		if (readError) {
			break;
		}
 		std::cout << entity.getName() << " : " << position.x << " : " << position.y << " : " << position.z << std::endl;
	}
}
