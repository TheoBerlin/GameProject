#include "LevelParser.h"

#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"
#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include <Game/Components/RollNullifier.h>

void LevelParser::readEntityTargets(EntityManager * entityManager)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Target"].size();

	if (targetSize != 0) {
		model = ModelLoader::loadModel("./Game/assets/droneTarget.fbx");
	}

	for (int i = 0; i < targetSize; i++)
	{
		json::json& target = jsonFile["Target"][i];
		Entity* entity;
		glm::vec3 position;
		std::vector<KeyPoint> path;

		//Every object requires a name
		if (!target["Name"].empty() && target["Name"].is_string()) {
			std::string name = target["Name"];
			entity = entityManager->addTracedEntity(name);

			if (!target["Position"].empty()) {
				readPosition(target, entity, position);
			} else if (!target["Path"].empty()) {
				readPath(target, entity, path);
			}
		} else {
			LOG_ERROR("%s: An object is missing a name or name is not a string", CLASS_NAME);
			break;
		}

		// If the target is mobile
		if (!path.empty()) {
			PathTreader* treader = new PathTreader(entity, path);
			treader->startTreading();

			position = path.at(0).Position;

			new RollNullifier(entity);
		}

		Transform* transform = entity->getTransform();

		transform->setPosition(position);
		transform->setScale(0.25f);
		transform->setForward(glm::vec3(0.0f, 0.0f, 1.0f));

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

			if (!box["Position"].empty()) {
				readPosition(box, entity, position);
			}
		} else {
			LOG_ERROR("%s: An object is missing a name or name is not a string", CLASS_NAME);
			break;
		}

		entity->getTransform()->setPosition(position);
		entity->getTransform()->setScale(0.25f);
		entity->setModel(model);
	}
}

void LevelParser::readEntityWalls(EntityManager * entityManager)
{
	//Add read for walls
}

void LevelParser::readEntityFloor(EntityManager * entityManager)
{
	Model *model = nullptr;
	model = ModelLoader::loadModel("./Game/assets/floor.fbx");

	Entity* entity;
	glm::vec3 position;

	json::json& floor = jsonFile["Floor"];

	entity = entityManager->addEntity();

	if (!floor["Position"].empty()) {
		readPosition(floor, entity, position);
	}

	entity->getTransform()->setPosition(position);
	entity->setModel(model);
}

void LevelParser::readPosition(json::json& file, Entity* entity, glm::vec3& position)
{
	// Iterate through position components
	for (int j = 0; j < 3; j++) {
		// If object exists go ahead otherwise write a default position
		if (!file["Position"][j].empty()) {
			try {
				position[j] = file["Position"][j];
			}
			catch (const std::exception& e) {
				LOG_ERROR("%s: at '%s' : %s", CLASS_NAME, entity->getName().c_str(), e.what());
				break;
			}
		} else {
			// Default position
			position[j] = 0.0;
			LOG_WARNING("%s: Did not find Position %d (0=X, 1=Y, 2=Z) value at '%s', defaulting to 0", CLASS_NAME, j, entity->getName().c_str());
		}
	}
}

void LevelParser::readPath(json::json& file, Entity* entity, std::vector<KeyPoint>& path)
{
	unsigned int pathSize = file["Path"].size();
	KeyPoint keyPoint;

	// Iterate through key points
	for (unsigned int pointIndex = 0; pointIndex < pathSize; pointIndex += 1) {
		// Read position
		if (!file["Path"][pointIndex]["Position"].empty()) {
			readPosition(file["Path"][pointIndex], entity, keyPoint.Position);
		}

		// Read time
		try {
			keyPoint.t = file["Path"][pointIndex]["Time"];
		}
		catch (const std::exception& e) {
			LOG_ERROR("%s: at '%s' : %s", CLASS_NAME, entity->getName().c_str(), e.what());
			break;
		}

		path.push_back(keyPoint);
	}
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
		readEntityFloor(entityManager);
	}
	else
	{
		LOG_ERROR("Can not open file: %s", file.c_str());
	}

}
