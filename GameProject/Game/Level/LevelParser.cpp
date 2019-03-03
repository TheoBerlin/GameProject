#include "LevelParser.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Game/Components/RollNullifier.h>
#include <Utils/Logger.h>
#include <Utils/Utils.h>

void LevelParser::readEntityTargets(Level& level)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Target"].size();

	if (targetSize != 0) {
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
		shapeData.push_back(droneData);

		// Main body need to be roteded and repositioned for us to be happy.
		CollisionHandler::ShapeData droneBodyData;
		droneBodyData.category = CATEGORY::DRONE_BODY;
		droneBodyData.scale = entity->getTransform()->getScale();
		droneBodyData.offset = glm::vec3(0.f, -0.05f, 0.f);
		droneBodyData.rotation = Utils::toQuaternion(0.f, glm::pi<float>() / 8.f, 0.f);
		droneBodyData.index = 4;
		shapeData.push_back(droneBodyData);
		
		// Eye of the drone.
		CollisionHandler::ShapeData droneEyeData;
		droneEyeData.category = CATEGORY::DRONE_EYE;
		droneEyeData.scale = entity->getTransform()->getScale();
		droneEyeData.index = 2;

		// Inner eye of the drone.
		CollisionHandler::ShapeData droneEyeData2;
		droneEyeData2.category = CATEGORY::DRONE_EYE;
		droneEyeData2.scale = entity->getTransform()->getScale();
		droneEyeData2.index = 3;
		shapeData.push_back(droneEyeData);
		
		level.collisionHandler->addCollisionToEntity(entity, shapeData, false);

		//level.collisionHandler->addCollisionToEntity(entity, CATEGORY::DRONE_BODY, false, Utils::toQuaternion(0.f, glm::pi<float>()/8.f, 0.f));
		//*(categories[2]) = CATEGORY::DRONE_EYE;
	}
}

void LevelParser::readEntityBoxes(Level& level)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Boxes"].size();

	if (targetSize != 0) {
		model = ModelLoader::loadModel("./Game/assets/Cube.fbx", level.collisionHandler);
	}

	for (int i = 0; i < targetSize; i++)
	{
		json::json& box = jsonFile["Boxes"][i];
		Entity* entity;
		glm::vec3 position;
		//Every object requires a name
		if (!box["Name"].empty() && box["Name"].is_string()) {
			std::string name = box["Name"];
			entity = level.entityManager->addEntity();

			if (!box["Position"].empty()) {
				readVec3(box["Position"], position);
			}
		} else {
			LOG_ERROR("An object is missing a name or name is not a string");
			break;
		}

		entity->getTransform()->setPosition(position);
		entity->getTransform()->setScale(0.5f);
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
	level.collisionHandler->addCollisionToEntity(entity, CATEGORY::NO_COLLISION);
}

void LevelParser::readPlayer(Level& level)
{
	json::json& player = jsonFile["Player"];

	readCameraSetting(player["OversightCamera"], level.player.oversightCamera);
	readCameraSetting(player["ArrowCamera"], level.player.arrowCamera);
	readCameraSetting(player["ReplayCamera"], level.player.replayCamera);
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
