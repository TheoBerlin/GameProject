#include "LevelParser.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Game/Components/RollNullifier.h>
#include <Utils/Logger.h>
#include <Game/components/Hover.h>
#include <Utils/Utils.h>

void LevelParser::readEntityTargets(Level& level)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Target"].size();

	if (targetSize != 0) {
		model->setName("droneTarget");
		model = ModelLoader::loadModel("./Game/assets/droneTarget.fbx", level.collisionHandler);
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
			entity = level.entityManager->addTracedEntity(name);

			if (!target["Position"].empty()) {
				readVec3(target["Position"], position);
			} else if (!target["Path"].empty()) {
				readPath(target, entity, path);
			}
		} else {
			LOG_ERROR("An object is missing a name or name is not a string");
			break;
		}

		if (!path.empty()) {
			// The target is mobile
			level.targetManager->addMovingTarget(entity, path);
		} else {
			// The target is static
			level.targetManager->addStaticTarget(entity, position);
		}

		entity->setModel(model);
		std::vector<CollisionHandler::ShapeData> shapeData;

		// If no spcific data than this will be used.
		CollisionHandler::ShapeData droneData;
		droneData.category = CATEGORY::DRONE_BODY;
		droneData.scale = entity->getTransform()->getScale();
		droneData.localScale = glm::vec3(0.8f);
		shapeData.push_back(droneData);

		// Eye of the drone.
		CollisionHandler::ShapeData droneEyeData;
		droneEyeData.category = CATEGORY::DRONE_EYE;
		droneEyeData.scale = entity->getTransform()->getScale();
		droneEyeData.localScale = glm::vec3(0.8f);
		droneEyeData.name = "Eye";
		shapeData.push_back(droneEyeData);
		
		level.collisionHandler->addCollisionToEntity(entity, shapeData, false);
	}
}

void LevelParser::readEntityBoxes(Level& level)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Boxes"].size();

	if (targetSize != 0) {
		model->setName("Cube");
		model = ModelLoader::loadModel("./Game/assets/Cube.fbx", level.collisionHandler);
	}

	for (int i = 0; i < targetSize; i++)
	{
		json::json& box = jsonFile["Boxes"][i];
		Entity* entity;
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation;
		//Every object requires a name
		if (!box["Name"].empty() && box["Name"].is_string()) {
			std::string name = box["Name"];
			entity = level.entityManager->addEntity();
			entity->setName(name);

			if (!box["Position"].empty()) {
				readVec3(box["Position"], position);
				entity->getTransform()->setPosition(position);
			}
			if (!box["Scale"].empty()) {
				readVec3(box["Scale"], scale);
				entity->getTransform()->setScale(scale);
			}
			if (!box["Rotation"].empty()) {
				readVec3(box["Rotation"], rotation);
				entity->getTransform()->setRotation(rotation);
			}
		} else {
			LOG_ERROR("An object is missing a name or name is not a string");
			break;
		}

		entity->setModel(model);
		level.collisionHandler->addCollisionToEntity(entity, CATEGORY::STATIC);
	}
}

void LevelParser::readEntityWalls(Level& level)
{
	//Add read for walls
}

void LevelParser::readEntityFloor(Level& level)
{
	Model *model = nullptr;
	model = ModelLoader::loadModel("./Game/assets/floor.fbx", level.collisionHandler);

	Entity* entity;
	glm::vec3 position;

	json::json& floor = jsonFile["Floor"];

	entity = level.entityManager->addEntity();

	if (!floor["Position"].empty()) {
		readVec3(floor["Position"], position);
	}

	entity->getTransform()->setPosition(position);
	entity->setModel(model);
	level.collisionHandler->addCollisionToEntity(entity, CATEGORY::STATIC);
}

void LevelParser::readPlayer(Level& level)
{
	json::json& player = jsonFile["Player"];

	readCameraSetting(player["OversightCamera"], level.player.oversightCamera);
	readCameraSetting(player["ArrowCamera"], level.player.arrowCamera);
	readCameraSetting(player["ReplayCamera"], level.player.replayCamera);
}

void LevelParser::writeEntityBoxes(Level & level)
{
	int nrOfBoxes = 0;
	for (int i = 0; i < level.entityManager->getEntitySize(); i++) {
		bool found = false; 
		for (int j = 0; j < level.targetManager->getMovingTargets().size(); j++) {
			if (level.targetManager->getMovingTargets()[j].pathTreader->getHost() == level.entityManager->getEntity(i))
				found = true;
		}
		for (int j = 0; j < level.targetManager->getStaticTargets().size(); j++) {
			if (level.targetManager->getStaticTargets()[j].hoverAnimation->getHost() == level.entityManager->getEntity(i))
				found = true;
		}
		if (!found) {
			jsonFile["Boxes"][nrOfBoxes]["Name"] = level.entityManager->getEntity(i)->getName();
			jsonFile["Boxes"][nrOfBoxes]["Position"] = { level.entityManager->getEntity(i)->getTransform()->getPosition().x, level.entityManager->getEntity(i)->getTransform()->getPosition().y, level.entityManager->getEntity(i)->getTransform()->getPosition().z };
			jsonFile["Boxes"][nrOfBoxes]["Scale"] = { level.entityManager->getEntity(i)->getTransform()->getScale().x, level.entityManager->getEntity(i)->getTransform()->getScale().y, level.entityManager->getEntity(i)->getTransform()->getScale().z };
			jsonFile["Boxes"][nrOfBoxes]["Rotation"] = { level.entityManager->getEntity(i)->getTransform()->getRotation().x, level.entityManager->getEntity(i)->getTransform()->getRotation().y, level.entityManager->getEntity(i)->getTransform()->getRotation().z };
			nrOfBoxes++;
		}
	}
}

void LevelParser::writeEntityTargets(Level & level)
{
	int nrOfTargets = 0;
	for (int i = 0; i < level.entityManager->getEntitySize(); i++) {
		bool found = false;
		for (int j = 0; j < level.targetManager->getMovingTargets().size(); j++) {
			if (level.targetManager->getMovingTargets()[j].pathTreader->getHost() == level.entityManager->getEntity(i)) {
				jsonFile["Target"][nrOfTargets]["Name"] = level.entityManager->getEntity(i)->getName();
				jsonFile["Target"][nrOfTargets]["Position"] = { level.entityManager->getEntity(i)->getTransform()->getPosition().x, level.entityManager->getEntity(i)->getTransform()->getPosition().y, level.entityManager->getEntity(i)->getTransform()->getPosition().z };
				jsonFile["Target"][nrOfTargets]["Scale"] = { level.entityManager->getEntity(i)->getTransform()->getScale().x, level.entityManager->getEntity(i)->getTransform()->getScale().y, level.entityManager->getEntity(i)->getTransform()->getScale().z };
				jsonFile["Target"][nrOfTargets]["Rotation"] = { level.entityManager->getEntity(i)->getTransform()->getRotation().x, level.entityManager->getEntity(i)->getTransform()->getRotation().y, level.entityManager->getEntity(i)->getTransform()->getRotation().z };
				for (int k = 0; k < level.targetManager->getMovingTargets()[j].pathTreader->getPath().size(); k++)
					jsonFile["Target"][nrOfTargets]["Path"][k] = { level.targetManager->getMovingTargets()[j].pathTreader->getPath()[k].Position.x, level.targetManager->getMovingTargets()[j].pathTreader->getPath()[k].Position.y, level.targetManager->getMovingTargets()[j].pathTreader->getPath()[k].Position.z };
				nrOfTargets++;
			}
		}
		for (int j = 0; j < level.targetManager->getStaticTargets().size(); j++) {
			if (level.targetManager->getStaticTargets()[j].hoverAnimation->getHost() == level.entityManager->getEntity(i)) {
				jsonFile["Target"][nrOfTargets]["Name"] = level.entityManager->getEntity(i)->getName();
				jsonFile["Target"][nrOfTargets]["Position"] = { level.entityManager->getEntity(i)->getTransform()->getPosition().x, level.entityManager->getEntity(i)->getTransform()->getPosition().y, level.entityManager->getEntity(i)->getTransform()->getPosition().z };
				jsonFile["Target"][nrOfTargets]["Scale"] = { level.entityManager->getEntity(i)->getTransform()->getScale().x, level.entityManager->getEntity(i)->getTransform()->getScale().y, level.entityManager->getEntity(i)->getTransform()->getScale().z };
				jsonFile["Target"][nrOfTargets]["Rotation"] = { level.entityManager->getEntity(i)->getTransform()->getRotation().x, level.entityManager->getEntity(i)->getTransform()->getRotation().y, level.entityManager->getEntity(i)->getTransform()->getRotation().z };
				nrOfTargets++;
			}
		}
	}
}

void LevelParser::writePlayer(Level & level)
{
	jsonFile["Player"]["OversightCamera"]["Position"] = { level.player.oversightCamera.position.x, level.player.oversightCamera.position.y, level.player.oversightCamera.position.z };
	jsonFile["Player"]["OversightCamera"]["Direction"] = { level.player.oversightCamera.direction.x, level.player.oversightCamera.direction.y, level.player.oversightCamera.direction.z };
	jsonFile["Player"]["OversightCamera"]["Offset"] = { level.player.oversightCamera.offset.x, level.player.oversightCamera.offset.y, level.player.oversightCamera.offset.z };
	jsonFile["Player"]["OversightCamera"]["FOV"] = level.player.oversightCamera.FOV;

	jsonFile["Player"]["ArrowCamera"]["Position"] = { level.player.arrowCamera.position.x, level.player.arrowCamera.position.y, level.player.arrowCamera.position.z };
	jsonFile["Player"]["ArrowCamera"]["Direction"] = { level.player.arrowCamera.direction.x, level.player.arrowCamera.direction.y, level.player.arrowCamera.direction.z };
	jsonFile["Player"]["ArrowCamera"]["Offset"] = { level.player.arrowCamera.offset.x, level.player.arrowCamera.offset.y, level.player.arrowCamera.offset.z };
	jsonFile["Player"]["ArrowCamera"]["FOV"] = level.player.arrowCamera.FOV;

	jsonFile["Player"]["ReplayCamera"]["Position"] = { level.player.replayCamera.position.x, level.player.replayCamera.position.y, level.player.replayCamera.position.z };
	jsonFile["Player"]["ReplayCamera"]["Direction"] = { level.player.replayCamera.direction.x, level.player.replayCamera.direction.y, level.player.replayCamera.direction.z };
	jsonFile["Player"]["ReplayCamera"]["Offset"] = { level.player.replayCamera.offset.x, level.player.replayCamera.offset.y, level.player.replayCamera.offset.z };
	jsonFile["Player"]["ReplayCamera"]["FOV"] = level.player.replayCamera.FOV;
}

void LevelParser::readMetadata(Level& level)
{
	json::json& metadata = jsonFile["Metadata"];

	// Read optimal time for scoreManager
	level.scoreManager->setOptimalTime(readValue<float>(metadata, "OptimalTime"));
}

void LevelParser::readVec3(json::json& file, glm::vec3& vec)
{
	// Iterate through position components
	for (int j = 0; j < 3; j++) {
		// If object exists go ahead otherwise write a default position
		if (!file[j].empty()) {
			try {
				vec[j] = file[j];
			}
			catch (const std::exception& e) {
				LOG_ERROR("Failed to read vector component: %s", e.what());
				break;
			}
		} else {
			// Default component
			vec[j] = 1.0f;
			LOG_WARNING("Did not find Vec3 component %d (0=X, 1=Y, 2=Z), defaulting to 1", j);
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
			readVec3(file["Path"][pointIndex]["Position"], keyPoint.Position);
		}

		// Read time
		try {
			keyPoint.t = file["Path"][pointIndex]["Time"];
		}
		catch (const std::exception& e) {
			LOG_ERROR("'%s' : %s", entity->getName().c_str(), e.what());
			break;
		}

		path.push_back(keyPoint);
	}
}

void LevelParser::readCameraSetting(json::json& file, CameraSetting& camera)
{
	readVec3(file["Position"], camera.position);
	readVec3(file["Direction"], camera.direction);
	readVec3(file["Offset"], camera.offset);

	// Read FOV
	try {
		camera.FOV = file["FOV"];
	}
	catch (const std::exception& e) {
		LOG_ERROR("Failed to FOV: %s", e.what());
		camera.FOV = 75.0f;
	}

	camera.direction = glm::normalize(camera.direction);
}

void LevelParser::createCollisionBodies(Level& level)
{
	// Start at 1 to give space for a player later on.
	int bodiesNeeded = 2;
	bodiesNeeded += jsonFile["Target"].size();
	bodiesNeeded += jsonFile["Boxes"].size();

	level.collisionHandler->createCollisionBodies(bodiesNeeded);
}

void LevelParser::readLevel(std::string file, Level& level)
{
	std::ifstream iFile;
	iFile.open(file);
	if (iFile.is_open())
	{
		try {
			iFile >> jsonFile;
		}
		catch (const std::exception e) {
			LOG_ERROR("Failed to read JSON file with error: %s", e.what());
			return;
		}

		// Read metadata
		readMetadata(level);

		// Create collision bodies to collisionHandler
		createCollisionBodies(level);

		// Add entites to entityManager
		readEntityTargets(level);
		readEntityBoxes(level);
		readEntityWalls(level);
		readEntityFloor(level);
		readPlayer(level);
	}
	else
	{
		LOG_ERROR("Can not open file: %s", file.c_str());
	}

}

void LevelParser::writeLevel(std::string file, Level & level)
{
	json::json j;
	jsonFile = j;

	writeEntityBoxes(level);
	writeEntityTargets(level);
	writePlayer(level);

	std::ofstream oFile;
	oFile.open(file);
	oFile << std::setw(4) << jsonFile << std::endl;
}
