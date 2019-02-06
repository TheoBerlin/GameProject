#include "LevelParser.h"

/*void LevelParser::writePosition(int index, glm::vec3 position)
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
}*/


void LevelParser::readEntityTargets(EntityManager * entityManager)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Target"].size();

	if (targetSize != 0) {
		model = ModelLoader::loadModel("./Game/assets/Target.fbx");
	}

	for (int i = 0; i < targetSize; i++)
	{
		Entity* entity;
		glm::vec3 position;
		//Every object requires a name
		if (!jsonFile["Target"][i]["Name"].empty() && jsonFile["Target"][i]["Name"].is_string()) {
			std::string name = jsonFile["Target"][i]["Name"];
			entity = entityManager->addTracedEntity(name);

			//Start on X then loop through all positions
			for (int j = 0; j < 3; j++) {
				//If object exists go ahead otherwise do default position
				if (!jsonFile["Target"][i]["Position"][j].empty()) {
					try {
						position[j] = jsonFile["Target"][i]["Position"][j];
					}
					catch (const std::exception& e) {
						LOG_ERROR("%s: at '%s' : %s", CLASS_NAME, entity->getName().c_str(), e.what());
						break;
					}
				}
				else {
					//Default position
					position[j] = 0.0;
					LOG_WARNING("%s: Did not find Position %d (0=X, 1=Y, 2=Z) value at '%s', defaulting to 0", CLASS_NAME, j, entity->getName().c_str());
				}
			}
		}
		else {
			LOG_ERROR("%s: An object is missing a name or name is not a string", CLASS_NAME);
			break;
		}
		entity->getMatrix()->setPosition(position);
		entity->setModel(model);
	}
}

void LevelParser::readEntityBoxes(EntityManager * entityManager)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Boxes"].size();

	if (targetSize != 0) {
		model = ModelLoader::loadModel("./Game/assets/Cube.fbx");
	}

	for (int i = 0; i < targetSize; i++)
	{
		json::json& box = jsonFile["Boxes"][i];
		Entity* entity;
		glm::vec3 position;
		//Every object requires a name
		if (!box["Name"].empty() && box["Name"].is_string()) {
			std::string name = box["Name"];
			entity = entityManager->addEntity();
			//Start on X then loop through all positions
			for (int j = 0; j < 3; j++) {
				//If object exists go ahead otherwise do default position
				if (!box["Position"][j].empty()) {
					try {
						position[j] = box["Position"][j];
					}
					catch (const std::exception& e) {
						LOG_ERROR("%s: at '%s' : %s", CLASS_NAME, entity->getName().c_str(), e.what());
						break;
					}
				}
				else {
					//Default position
					position[j] = 0.0;
					LOG_WARNING("%s: Did not find Position %d (0=X, 1=Y, 2=Z) value at '%s', defaulting to 0", CLASS_NAME, j, entity->getName().c_str());
				}
			}
		}
		else {
			LOG_ERROR("%s: An object is missing a name or name is not a string", CLASS_NAME);
			break;
		}
		entity->getMatrix()->setPosition(position);
		entity->setModel(model);
	}
}

void LevelParser::readEntityWalls(EntityManager * entityManager)
{
	//Add read for walls
}

void LevelParser::readEntityArrow(EntityManager * entityManager)
{
	//add read for the arrow
}

void LevelParser::readEntites(std::string file, EntityManager *entityManager)
{
	std::ifstream iFile;
	iFile.open(file);
	if (iFile.is_open())
	{
		try {
			iFile >> jsonFile;
		}
		catch (const std::exception e) {
			LOG_ERROR("%s: Failed to read JSON file with error: %s", CLASS_NAME, e.what());
			return;
		}

		readEntityTargets(entityManager);
		readEntityBoxes(entityManager);
		readEntityWalls(entityManager);
		readEntityArrow(entityManager);
	}
	else
	{
		LOG_ERROR("Can not open file: %s", file.c_str());
	}

}
