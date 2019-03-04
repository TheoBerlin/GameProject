#include "LevelParser.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Game/Components/RollNullifier.h>
#include <Engine/AssetManagement/Mesh.h>
#include <Engine/Rendering/GLAbstraction/RenderingResources.h>
#include <Utils/Logger.h>

void LevelParser::readEntityTargets(Level& level)
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
		level.collisionHandler->addCollisionToEntity(entity, SHAPE::DRONE);
	}
}

void LevelParser::readEntityBoxes(Level& level)
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
		level.collisionHandler->addCollisionToEntity(entity, SHAPE::BOX);
	}
}

void LevelParser::readEntityWalls(Level& level)
{
	// Pre determined height
	float height = 5.0f;

	// Load points from level file
	json::json& file = jsonFile["Walls"];
	std::vector<glm::vec3> points;
	glm::vec2 point(0.0f);
	if (!file.empty()) {
		for (unsigned i = 0; i < file.size(); i++)
		{
			readVec2(file[i], point);
			points.push_back({ point.x, 0.0f, point.y });
		}
	}
	else {
		LOG_ERROR("NO WALLS FOUND");
		return;
	}

	std::vector<glm::vec2> scales;

	Model* model = createQuat();

	std::vector<glm::mat4> mats;
	for (int i = 0; i < points.size(); i++)
	{
		Entity* entity = level.entityManager->addTracedEntity("WallPoint" + std::to_string(i));
		glm::vec3* p1 = &points[i];
		glm::vec3* p2 = &points[(i + 1) % (points.size())];
		
		glm::vec3 width = *p2 - *p1;

		float angle = acosf(glm::dot(glm::normalize(width), { 1.0f, 0.0f, 0.0f }));
		if (glm::dot(glm::normalize(width), { 0.0f, 0.0f, -1.0f }) < 0.0f)
			angle = -angle;
		entity->getTransform()->setRotation(glm::vec3(0.0f, angle, 0.0f));

		float dist = glm::length(width);
		entity->getTransform()->setScale(glm::vec3(dist, height, 1.0f));

		entity->getTransform()->setPosition(*p1);

		mats.push_back(entity->getTransform()->getMatrix());
		entity->setModel(model);

		scales.push_back(glm::vec2(glm::length(width), height));
	}

	Mesh* mesh = model->getMesh(0);

	AttributeLayout matLayout;
	matLayout.push(4, 1);
	matLayout.push(4, 1);
	matLayout.push(4, 1);
	matLayout.push(4, 1);
	mesh->addBuffer(&mats[0][0], mats.size() * sizeof(glm::mat4), matLayout);

	AttributeLayout scaleLayout;
	scaleLayout.push(2, 1);
	mesh->addBuffer(&scales[0], scales.size() * sizeof(glm::vec2), scaleLayout);


	ModelLoader::addModel("wall", model);
}

void LevelParser::readEntityFloor(Level& level)
{
	Model *model = nullptr;
	model = ModelLoader::loadModel("./Game/assets/floor.fbx");

	Entity* entity;
	glm::vec3 position;

	json::json& floor = jsonFile["Floor"];

	entity = level.entityManager->addEntity();

	if (!floor["Position"].empty()) {
		readVec3(floor["Position"], position);
	}

	entity->getTransform()->setPosition(position);
	entity->setModel(model);
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
	// Start at 1 to give space for a player later on.
	int bodiesNeeded = 1;
	bodiesNeeded += jsonFile["Target"].size();
	bodiesNeeded += jsonFile["Boxes"].size();

	level.collisionHandler->createCollisionBodies(bodiesNeeded);
}

Model * LevelParser::createQuat()
{
	Model* quad = new Model();

	std::vector<Vertex>* vertices = new	std::vector<Vertex>();
	std::vector<GLuint>* indicies = new std::vector<GLuint>();

	Vertex vertex;
	vertex.Normal = glm::vec3(0.0, 0.0, 1.0);

	vertex.Position = glm::vec3(0.0, 0.0, 0.0);
	vertex.TexCoords = glm::vec2(0.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 0.0, 0.0);
	vertex.TexCoords = glm::vec2(1.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 1.0, 0.0);
	vertex.TexCoords = glm::vec2(1.0, 1.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(0.0, 1.0, 0.0);
	vertex.TexCoords = glm::vec2(0.0, 1.0);
	vertices->push_back(vertex);

	indicies->push_back(0);
	indicies->push_back(1);
	indicies->push_back(2);
	indicies->push_back(2);
	indicies->push_back(3);
	indicies->push_back(0);

	Mesh* quadMesh = new Mesh(vertices, indicies, 0, quad);
	quad->addMesh(quadMesh);

	Material mat;
	float f = 0.5f;
	Texture* tex = TextureManager::loadTexture("./Game/assets/textures/wallTex.png");
	mat.textures.push_back(tex);
	mat.Ks_factor = glm::vec4(40.0f);
	quad->addMaterial(mat);

	return quad;
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
