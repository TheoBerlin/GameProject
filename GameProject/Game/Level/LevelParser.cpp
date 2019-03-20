#include "LevelParser.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Engine/AssetManagement/Mesh.h>
#include <Engine/Rendering/GLAbstraction/RenderingResources.h>
#include <Game/Components/ArrowConfig.h>
#include <Game/Components/RollNullifier.h>
#include <Game/components/Hover.h>
#include <Utils/Logger.h>
#include <Utils/Utils.h>

void LevelParser::readEntityTargets(Level& level)
{
	Model *modelStatic = nullptr;
	Model *modelMoving = nullptr;
	//Get the size of the target entities
	unsigned int targetSize = jsonFile["Target"].size();

	if (targetSize != 0) {
		modelStatic = ModelLoader::loadModel("./Game/assets/droneTarget.fbx", level.collisionHandler);
		Display::get().getRenderer().addRenderingTarget(modelStatic, SHADERS::DRONE_SHADER);

		modelMoving = ModelLoader::loadModel("./Game/assets/droneTargetMoving.fbx", level.collisionHandler);
		Display::get().getRenderer().addRenderingTarget(modelMoving, SHADERS::DRONE_GHOST);
	}

	for (unsigned int i = 0; i < targetSize; i++)
	{
		json::json& target = jsonFile["Target"][i];
		Entity* entity;
		glm::vec3 position;
		glm::vec3 rotation;
		std::vector<KeyPoint> path;

		//Every object requires a name
		if (!target["Name"].empty() && target["Name"].is_string()) {
			std::string name = target["Name"];
			entity = level.entityManager->addTracedEntity(name);

			if (!target["Position"].empty()) {
				readVec3(target["Position"], position);
			}
			if (!target["Rotation"].empty()) {
				readVec3(target["Rotation"], rotation);
			}
			if (!target["Path"].empty()) {
				readPath(target, entity, path);
			}
		}

		else {
			LOG_ERROR("An object is missing a name or name is not a string");
			break;
		}

		if (!path.empty()) {
			// The target is mobile
			level.targetManager->addMovingTarget(entity, path);
		}

		else {
			// The target is static
			level.targetManager->addStaticTarget(entity, position);
			entity->getTransform()->setRotation(rotation);
		}
		entity->setModel(modelStatic);

		std::vector<CollisionHandler::ShapeData> shapeData;

		// If no spcific data then this will be used.
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

void LevelParser::readEntityProps(Level& level)
{
	//Get the size of the target entities
	int targetSize = jsonFile["Props"].size();

	std::vector<Model*> model;

	if (targetSize != 0) {
		for (unsigned int i = 0; i < targetSize; i++) {
			std::string name = jsonFile["Props"][i]["Model"];
			if (!ModelLoader::getModel(std::string("./Game/assets/") + name + std::string(".fbx"))) {
				model.push_back(ModelLoader::loadModel(std::string("./Game/assets/") + name + std::string(".fbx"), level.collisionHandler));
				model[i]->setName(name);
				Display::get().getRenderer().addRenderingTarget(model[i]);
			}
			else {
				model.push_back(ModelLoader::getModel(std::string("./Game/assets/") + name + std::string(".fbx")));
				model[i]->setName(name);
			}
		}
	}

	for (int i = 0; i < targetSize; i++)
	{
		json::json& box = jsonFile["Props"][i];
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
			if (!box["Scale"].empty()) {
				readVec3(box["Scale"], scale);
			}
		} else {
			LOG_ERROR("An object is missing a name or name is not a string");
			break;
		}

		entity->setModel(model[i]);
		level.collisionHandler->addCollisionToEntity(entity, CATEGORY::STATIC);
	}
}

void LevelParser::readEntityWalls(Level& level)
{
	// Load points from level file
	if (jsonFile.find("Walls") == jsonFile.end())
		return;
	try {
		json::json& file = jsonFile["Walls"];
		std::vector<std::vector<glm::vec3>> points;
		glm::vec2 point(0.0f);

		try {
			if (!file["WallInfo"]["Texture"].empty())
				level.levelStructure->setTexture(file["WallInfo"]["Texture"]);

			if (!file["WallInfo"]["Height"].empty())
				level.levelStructure->setWallHeight(file["WallInfo"]["Height"]);
		}
		catch (const std::exception& e) {
			LOG_ERROR("No Walls info found: %s", e.what());
		}

		if (!file.empty()) {
			for (unsigned group = 0; group < file["WallPoints"].size(); group++)
			{
				points.push_back(std::vector<glm::vec3>());
				for (unsigned i = 0; i < file["WallPoints"][group].size(); i++)
				{
					readVec2(file["WallPoints"][group][i], point);
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
	catch (const std::exception& e) {
		LOG_ERROR("Walls is outdated can't read walls: %s", e.what());
	}
}

void LevelParser::readPlayer(Level& level)
{
	json::json& player = jsonFile["Player"];

	readCameraSetting(player["OversightCamera"], level.player.oversightCamera);

	// Player arrow: Only read position and direction, get the FOV and offset from the arrow config file
	readVec3(player["ArrowCamera"]["Position"], level.player.arrowCamera.position);
	readVec3(player["ArrowCamera"]["Direction"], level.player.arrowCamera.direction);

	level.player.arrowCamera.offset = Arrow::minCamOffset;
	level.player.arrowCamera.FOV = Arrow::minFOV;
}

void LevelParser::writeEntityProps(Level & level)
{
	int nrOfProps = 0;
	for (int i = 0; i < level.entityManager->getEntitySize(); i++) {
		bool found = false;
		for (unsigned int j = 0; j < level.targetManager->getMovingTargets().size(); j++) {
			if (level.targetManager->getMovingTargets()[j].pathTreader->getHost() == level.entityManager->getEntity(i))
				found = true;
		}
		for (unsigned int j = 0; j < level.targetManager->getStaticTargets().size(); j++) {
			if (level.targetManager->getStaticTargets()[j].hoverAnimation->getHost() == level.entityManager->getEntity(i))
				found = true;
		}
		if (!found && level.entityManager->getEntity(i)->getName().substr(0, 9) != "WallPoint" && level.entityManager->getEntity(i)->getName().substr(0, 11) != "Placeholder") {
			Entity* entity = level.entityManager->getEntity(i);
			Transform* transform = entity->getTransform();

			jsonFile["Props"][nrOfProps]["Name"] = entity->getName();
			jsonFile["Props"][nrOfProps]["Model"] = entity->getModel()->getName().c_str();

			glm::vec3 position = transform->getPosition();
			jsonFile["Props"][nrOfProps]["Position"] = { position.x, position.y, position.z };

			glm::vec3 scale = transform->getScale();
			jsonFile["Props"][nrOfProps]["Scale"] = { scale.x, scale.y, scale.z };

			glm::vec3 orientation = transform->getYawPitchRoll();
			jsonFile["Props"][nrOfProps]["Rotation"] = { orientation.x, orientation.y, orientation.z };

			nrOfProps++;
		}
	}
}

void LevelParser::writeEntityTargets(Level & level)
{
	int nrOfTargets = 0;
	for (int i = 0; i < level.entityManager->getEntitySize(); i++) {
		Entity* entity = level.entityManager->getEntity(i);
		bool found = false;
		for (unsigned int j = 0; j < level.targetManager->getMovingTargets().size(); j++) {
			if (level.targetManager->getMovingTargets()[j].pathTreader->getHost() == entity) {
				// Write moving target
				Transform* transform = entity->getTransform();

				jsonFile["Target"][nrOfTargets]["Name"] = entity->getName();

				glm::vec3 position = transform->getPosition();
				jsonFile["Target"][nrOfTargets]["Position"] = { position.x, position.y, position.z };

				glm::vec3 scale = transform->getScale();
				jsonFile["Target"][nrOfTargets]["Scale"] = { scale.x, scale.y, scale.z };

				float yaw = transform->getYaw();
				jsonFile["Target"][nrOfTargets]["Yaw"] = yaw;

				// Write moving target's path
				for (unsigned int k = 0; k < level.targetManager->getMovingTargets()[j].pathTreader->getPath().size(); k++) {
					std::vector<KeyPoint> path = level.targetManager->getMovingTargets()[j].pathTreader->getPath();

					jsonFile["Target"][nrOfTargets]["Path"][k]["Position"] = { path[k].Position.x, path[k].Position.y, path[k].Position.z };
					jsonFile["Target"][nrOfTargets]["Path"][k]["Time"] = path[k].t;
				}
				nrOfTargets++;
			}
		}
		for (unsigned int j = 0; j < level.targetManager->getStaticTargets().size(); j++) {
			if (level.targetManager->getStaticTargets()[j].hoverAnimation->getHost() == entity) {
				// Write static target
				Transform* transform = entity->getTransform();

				jsonFile["Target"][nrOfTargets]["Name"] = entity->getName();

				glm::vec3 position = transform->getPosition();
				jsonFile["Target"][nrOfTargets]["Position"] = { position.x, position.y, position.z };

				glm::vec3 scale = transform->getScale();
				jsonFile["Target"][nrOfTargets]["Scale"] = { scale.x, scale.y, scale.z };

				glm::vec3 orientation = transform->getYawPitchRoll();
				jsonFile["Target"][nrOfTargets]["Rotation"] = { orientation.x, orientation.y, orientation.z };

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

	jsonFile["Metadata"]["OptimalTime"] = level.scoreManager->getOptimalTime();
}

void LevelParser::writeWalls(Level & level)
{
	int wallPointsOffset = 0;

	for (unsigned int i = 0; i < level.levelStructure->getWallGroupsIndex().size(); i++) {
		for (int j = 0; j < level.levelStructure->getWallGroupsIndex()[i]; j++) {
			glm::vec2 wallPoint = glm::vec2(level.levelStructure->getWallPoints()[wallPointsOffset + j].x, level.levelStructure->getWallPoints()[wallPointsOffset + j].z);
			jsonFile["Walls"]["WallPoints"][i][j] = { wallPoint.x, wallPoint.y };
		}
		wallPointsOffset += level.levelStructure->getWallGroupsIndex()[i];
	}
	jsonFile["Walls"]["WallInfo"]["Texture"] = level.levelStructure->getTexture();
	jsonFile["Walls"]["WallInfo"]["Height"] = level.levelStructure->getWallHeight();
}

void LevelParser::writeLight(Level & level)
{
	json::json& light = jsonFile["PointLights"];

	for (unsigned int i = 0; i < level.lightManager->getNrOfPointLights(); i++) {
		light[i]["Position"] = { level.lightManager->getPointLights()->at(i)->getPosition().x, level.lightManager->getPointLights()->at(i)->getPosition().y, level.lightManager->getPointLights()->at(i)->getPosition().z, level.lightManager->getPointLights()->at(i)->getPosition().w };
		light[i]["Color"] = { level.lightManager->getPointLights()->at(i)->getIntensity().x, level.lightManager->getPointLights()->at(i)->getIntensity().y, level.lightManager->getPointLights()->at(i)->getIntensity().z, level.lightManager->getPointLights()->at(i)->getIntensity().w };
		light[i]["Distance"] = level.lightManager->getPointLights()->at(i)->getDistance();
	}

	//check if light manager exists before writing to it
	if (level.lightManager) {
		glm::vec4 direction = level.lightManager->getDirectionalLight()->getDirection();
		glm::vec4 intesity = level.lightManager->getDirectionalLight()->getIntensity();
	
		jsonFile["DirectionalLight"]["Direction"] = { direction.x, direction.y, direction.z, direction.w };
		jsonFile["DirectionalLight"]["Intensity"] = { intesity.x, intesity.y, intesity.z, intesity.w };
	}
}

void LevelParser::readMetadata(Level& level)
{
	json::json& metadata = jsonFile["Metadata"];

	// Read optimal time for scoreManager
	level.scoreManager->setOptimalTime(readValue<float>(metadata, "OptimalTime"));
	level.scoreManager->setHighscore(readValue<unsigned>(metadata, "Highscore"));
}

void LevelParser::readLights(Level & level)
{
	int lightSize = jsonFile["PointLights"].size();
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 direction;
	glm::vec4 intensity;
	int distance;
	for (int i = 0; i < lightSize; i++) {
		json::json& light = jsonFile["PointLights"][i];
		readVec4(light["Position"], position);
		readVec4(light["Color"], color);
		distance = light["Distance"];

		level.lightManager->createPointLight(position, color, distance);
	}
	json::json& dLight = jsonFile["DirectionalLight"];
	readVec4(dLight["Direction"], direction);
	readVec4(dLight["Intensity"], intensity);

	level.lightManager->createDirectionalLight(direction, intensity, &level);
}

void LevelParser::readVec4(json::json & file, glm::vec4 & vec)
{
	// Iterate through position components
	for (int j = 0; j < 4; j++) {
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
			LOG_WARNING("Did not find Vec4 component %d (0=X, 1=Y, 2=Z, 3 = W), defaulting to 1", j);
		}
	}
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

json::json::value_type LevelParser::readValueGeneric(json::json & file, std::string value)
{
	if (!file[value].empty()) {
		return file[value];
	}
	else {
		LOG_WARNING("Value of %s not found in level!", value.c_str());
		json::json::value_type v = 0;
		return v;
	}
}

void LevelParser::createCollisionBodies(Level& level)
{
	try {
		json::json& walls = jsonFile["Walls"]["WallPoints"];

		int bodiesNeeded = 0;
		// Player
		bodiesNeeded += 1;
		// Floor (expected only one in this version)
		bodiesNeeded += 1;
		// Targets
		bodiesNeeded += jsonFile["Target"].size();
		bodiesNeeded += jsonFile["Props"].size();

		// Walls
		for (unsigned group = 0; group < walls.size(); group++)
			bodiesNeeded += walls[group].size();

		// Create bodies in collision handler
		level.collisionHandler->createCollisionBodies(bodiesNeeded);
	}
	catch (const std::exception& e) {
		LOG_ERROR("No walls found: %s", e.what());
		return;
	}
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
		readEntityProps(level);
		readEntityWalls(level);
		readPlayer(level);
		readLights(level);
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

	writeEntityProps(level);
	writeEntityTargets(level);
	writePlayer(level);
	writeWalls(level);
	writeLight(level);

	std::ofstream oFile;
	oFile.open(file);
	oFile << std::setw(4) << jsonFile << std::endl;
}

void LevelParser::readLevelInfo(std::string file, std::vector<std::string>& info)
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

		// Reset vector for new level
		info.clear();

		// Read needed info
		// Read target size
		info.push_back("Targets: " + std::to_string(jsonFile["Target"].size()));
		// Read optimal time
		info.push_back("OPTIMAL TIME: " + std::to_string((unsigned)readValue<float>(jsonFile["Metadata"], "OptimalTime")));
		// Read highscore
		info.push_back("HIGHSCORE: " + std::to_string(readValue<unsigned>(jsonFile["Metadata"], "Highscore")));
	}
	else
	{
		LOG_ERROR("Can not open file: %s", file.c_str());
	}
}

void LevelParser::writeScore(std::string file, Level & level)
{
	json::json& score = jsonFile["Metadata"]["Highscore"];

	score = level.scoreManager->getHighscore();

	std::ofstream oFile;
	oFile.open(file);
	oFile << std::setw(4) << jsonFile << std::endl;
}
