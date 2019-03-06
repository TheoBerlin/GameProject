#include "LevelParser.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Game/Components/RollNullifier.h>
#include <Engine/AssetManagement/Mesh.h>
#include <Engine/Rendering/GLAbstraction/RenderingResources.h>
#include <Utils/Logger.h>
#include <Utils/Utils.h>

void LevelParser::readEntityTargets(Level& level)
{
	Model *model = nullptr;
	//Get the size of the target entities
	int targetSize = jsonFile["Target"].size();

	if (targetSize != 0) {
		model = ModelLoader::loadModel("./Game/assets/droneTarget.fbx", level.collisionHandler);
		Display::get().getRenderer().addRenderingTarget(model, SHADERS::DRONE_SHADER);
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
		model = ModelLoader::loadModel("./Game/assets/Cube.fbx", level.collisionHandler);
		Display::get().getRenderer().addRenderingTarget(model);
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
	// Load points from level file
	if (jsonFile.find("Walls") == jsonFile.end())
		return;
	json::json& file = jsonFile["Walls"];
	std::vector<std::vector<glm::vec3>> points;
	glm::vec2 point(0.0f);
	if (!file.empty()) {
		for (unsigned group = 0; group < file.size(); group++)
		{
			points.push_back(std::vector<glm::vec3>());
			for (unsigned i = 0; i < file[group].size(); i++)
			{
				readVec2(file[group][i], point);
				points[group].push_back({ point.x, 0.0f, point.y });
			}
		}
		level.levelStructure->createWalls(level, points);
	}
	else {
		LOG_WARNING("No walls found, walls will not be created.");
		return;
	}
}

void LevelParser::readEntityFloor(Level& level)
{
	Model *model = nullptr;

	//Get the size of the target entities
	int targetSize = jsonFile["Floor"].size();

	if (targetSize != 0) {
		model = ModelLoader::loadModel("./Game/assets/floor.fbx", level.collisionHandler);
		Display::get().getRenderer().addRenderingTarget(model);
	}

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

void LevelParser::readVec2(json::json& file, glm::vec2& vec)
{
	// Iterate through position components
	for (int j = 0; j < 2; j++) {
		// If object exists go ahead otherwise write a default position
		if (!file[j].empty()) {
			try {
				vec[j] = file[j];
			}
			catch (const std::exception& e) {
				LOG_ERROR("Failed to read vector component: %s", e.what());
				break;
			}
		}
		else {
			// Default component
			vec[j] = 1.0f;
			LOG_WARNING("Did not find Vec2 component %d (0=X, 1=Z), defaulting to 1", j);
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
	// Start at 2 to give space for a player and floor later on.
	int bodiesNeeded = 2;
	bodiesNeeded += jsonFile["Target"].size();
	bodiesNeeded += jsonFile["Boxes"].size();

	if (jsonFile.find("Walls") != jsonFile.end())
	{
		bodiesNeeded += jsonFile["Walls"][0].size();
		bodiesNeeded += jsonFile["Walls"][1].size();
	}
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
