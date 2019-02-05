#include "LevelParser.h"

void LevelParser::writePosition(int index, glm::vec3 position)
{
	jsonFile[index]["X"] = position.x;
	jsonFile[index]["Y"] = position.y;
	jsonFile[index]["Z"] = position.z;
}

void LevelParser::writeName(int index, std::string name)
{
	jsonFile[index]["Name"] = name;
}

void LevelParser::writeToFile(std::string file, EntityManager *entityManager)
{
	for (int i = 0; i < entityManager->getEntitySize(); i++) {
		if (entityManager->getEntity(i)->getName().size() > 0) {
			writeName(i, entityManager->getEntity(i)->getName());
			writePosition(i, entityManager->getEntity(i)->getMatrix()->getPosition());
		}
		else {
			LOG_WARNING("%s: Entity does not have a name at %d", CLASS_NAME, i);
		}
	}

	std::ofstream oStream(file);
	oStream << std::setw(4) << jsonFile << std::endl;
}


void LevelParser::read(std::string file, EntityManager *entityManager)
{
	std::ifstream iFile(file);
	try {
		iFile >> jsonFile;
	}
	catch (const std::exception e) {
		LOG_ERROR("%s: Failed to read JSON file with error: %s", CLASS_NAME, e.what());
		return;
	}

	//Get amount of arrays to know how many entities to be made
	int size = jsonFile.size();

	for (int i = 0; i < size; i++)
	{
		Entity* entity = new Entity();
		glm::vec3 position;
		//Every object requires a name
		if (!jsonFile[i]["Name"].empty() && jsonFile[i]["Name"].is_string()) {
			entity->setName(jsonFile[i]["Name"]);

			//Start on X then loop through all positions
			std::string axis = "X";
			for (int j = 0; j < 3; j++) {
				//If object exists go ahead otherwise do default position
				if (!jsonFile[i][axis].empty()) {
					try {
						position[j] = jsonFile[i][axis];
					}
					catch (const std::exception& e) {
						LOG_ERROR("%s: at '%s' : %s", CLASS_NAME, entity->getName().c_str(), e.what());
						break;
					}
				}
				else {
					//Default position
					position[j] = 0.0;
					LOG_WARNING("%s: Did not find '%s' value at '%s', defaulting to 0", CLASS_NAME, axis.c_str(), entity->getName().c_str());
				}
				//X -> Y -> Z
				axis[0] += 1;
			}
		}
		else {
			LOG_ERROR("%s: An object is missing a name or name is not a string", CLASS_NAME);
			break;
		}
		entity->getMatrix()->setPosition(position);
		entityManager->addEntity(entity);
	}
}
