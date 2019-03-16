#include "LevelStructure.h"

#include "Engine/AssetManagement/Model.h"
#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/Renderer.h"

#include <Engine/AssetManagement/Mesh.h>
#include <Engine/Rendering/GLAbstraction/RenderingResources.h>
#include <Engine/AssetManagement/TextureManager.h>
#include <Engine/AssetManagement/ModelLoader.h>
#include <Game/Level/Level.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Entity/Transform.h>

#include <Utils/Utils.h>

#include <Utils/Logger.h>

LevelStructure::LevelStructure()
{
	this->height = 5.f;
	spawnedPoints = 0;
	textureFile = "wallTex";
}


LevelStructure::~LevelStructure()
{
}

void LevelStructure::createWalls(Level& level, std::vector<std::vector<glm::vec3>>& points)
{
	this->quad = createQuad();
	if (!level.isEditor) {
		this->infPlane = createInfPlane();
		this->roofPlane = createRoofPlane();
	}
	// Create a wall group for each set
	for (unsigned i = wallGroupsIndex.size(); i < points.size(); i++)
	{
		this->wallGroupsIndex.push_back((int)(points[i].size()));
		createWallGroup(level, points[i]);
	}
	// Create wall buffer
	createWallBuffers();

	if (!level.isEditor) {
		// Create infinity plane entity
		createInfinityPlane(level);
		// Create roof entity
		createRoof(level);
	}
}

void LevelStructure::createWallGroup(Level & level, std::vector<glm::vec3>& points, bool invertNormal)
{
	Model* model = this->quad;
	Mesh* mesh = model->getMesh(0);

	std::vector<glm::mat4> mats;
	for (unsigned i = 0; i < points.size(); i++)
	{
		Entity* entity = level.entityManager->addTracedEntity("WallPoint" + std::to_string(spawnedPoints));
		Transform* trans = entity->getTransform();
		glm::vec3* p1 = &points[i];
		glm::vec3* p2 = &points[(i + 1) % (points.size())];
		if (invertNormal)
		{
			glm::vec3* temp = p1;
			p1 = p2;
			p2 = temp;
		}

		glm::vec3 width = *p2 - *p1;

		float angle = acosf(glm::dot(glm::normalize(width), { 1.0f, 0.0f, 0.0f }));
		if (glm::dot(glm::normalize(width), { 0.0f, 0.0f, -1.0f }) < 0.0f)
			angle = -angle;
		trans->setRotation(glm::vec3(angle, 0.0f, 0.0f));

		float dist = glm::length(width);
		glm::vec3 scale = glm::vec3(dist, this->height, 1.0f);
		trans->setScale(scale);

		trans->setPosition(*p1);

		mats.push_back(trans->getMatrix());
		entity->setModel(model);

		this->scales.push_back(glm::vec2(glm::length(width), this->height));

		std::vector<Vertex> vertex(mesh->getVerticies().size());
		unsigned index = 0;
		for (Vertex& v : mesh->getVerticies())
		{
			vertex[index++].Position = scale * v.Position;
		}

		// Save upper wall points
		this->wallPoints.push_back({ p1->x, this->height, p1->z });

		// Add collision
		level.collisionHandler->constructBoundingBox(model, &vertex[0], vertex.size(), "");
		trans->setScale(glm::vec3(1.f, 1.f, 1.f));
		level.collisionHandler->addCollisionToEntity(entity, CATEGORY::STATIC, false, glm::quat(1.f, 0.f, 0.f, 0.f), this->wallEntites.size());
		trans->setScale(scale);

		// Save entity pointer
		this->wallEntites.push_back(entity);
		spawnedPoints++;
	}
}

void LevelStructure::addWall(Level & level) {
	std::vector<glm::vec3> points = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
	this->wallGroupsIndex.push_back((int)(3));
	createWallGroup(level, points);

	std::vector<glm::mat4> mats;
	for (unsigned i = 0; i < this->wallEntites.size(); i++) {
		mats.push_back(this->wallEntites[i]->getTransform()->getMatrix());
	}

	Mesh* mesh = this->quad->getMesh(0);

	mesh->updateInstancingData(&mats[0][0], mats.size() * sizeof(glm::mat4), 0, 3);
}

glm::vec2 LevelStructure::calcWallTransform(const glm::vec3 & p1, const glm::vec3 & p2, Transform * trans)
{
	glm::vec3 width = p2 - p1;
	if (glm::length(width) > 0.0f) {
		float angle = acosf(glm::dot(glm::normalize(width), { 1.0f, 0.0f, 0.0f }));

		if (glm::dot(glm::normalize(width), { 0.0f, 0.0f, -1.0f }) < 0.0f)
			angle = -angle;
		trans->setRotation(glm::vec3(angle, 0.0f, 0.0f));

		float dist = glm::length(width);
		glm::vec3 scale = glm::vec3(dist, this->height, 1.0f);
		trans->setScale(scale);

		trans->setPosition(p1);
	} else {
		LOG_ERROR("Wall Width is equal to 0, point to avoid this");
	}
	return width;
}

void LevelStructure::addWallCollision(Entity * entity, CollisionHandler* ch)
{
	if (entity) {
		Transform* trans = entity->getTransform();
		glm::vec3 scale = trans->getScale();
		Model* model = entity->getModel();
		Mesh* mesh = model->getMesh(0);

		if (model) {
			std::vector<Vertex> vertex(mesh->getVerticies().size());
			unsigned index = 0;
			for (Vertex& v : mesh->getVerticies())
			{
				vertex[index++].Position = scale * v.Position;
			}

			// Add collision
			ch->constructBoundingBox(model, &vertex[0], vertex.size(), "");
			trans->setScale(glm::vec3(1.f, 1.f, 1.f));
			ch->addCollisionToEntity(entity, CATEGORY::STATIC, false, glm::quat(1.f, 0.f, 0.f, 0.f), this->wallEntites.size());
			trans->setScale(scale);
		}
	}
}

void LevelStructure::setTexture(std::string fileName) {
	textureFile = fileName;
}

std::string LevelStructure::getTexture() const {
	return textureFile;
}


void LevelStructure::removePoint(Level & level, int wallGroupIndex, unsigned pointIndex)
{
	if (wallGroupsIndex[wallGroupIndex] > 2) {

		int offset = 0;
		for (int i = 0; i < wallGroupIndex; i++) {
			offset += wallGroupsIndex[i];
		}

		int prev = (((pointIndex - offset - 1) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;
		int next = (((pointIndex - offset + 1) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;

		//Update transform of entity before removed
		Entity* entity = wallEntites[prev];
		Transform* trans = entity->getTransform();

		glm::vec3 p1(wallPoints[prev].x, 0.0, wallPoints[prev].z);
		glm::vec3 p2(wallPoints[next].x, 0.0, wallPoints[next].z);

		glm::vec2 width = this->calcWallTransform(p1, p2, trans);

		//Update scale of point before removed
		this->scales[prev] = glm::vec2(glm::length(width), this->height);

		//Erase point
		int wallToBeRemoved = (((pointIndex - offset) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;
		for (int j = 0; j < level.entityManager->getEntitySize(); j++) {
			if (wallEntites[wallToBeRemoved] == level.entityManager->getEntity(j))
				level.entityManager->removeEntity(j);
		}
		
		wallPoints.erase(wallPoints.begin() + wallToBeRemoved);
		scales.erase(scales.begin() + wallToBeRemoved);
		wallEntites.erase(wallEntites.begin() + wallToBeRemoved);
		wallGroupsIndex[wallGroupIndex]--;

		updateBuffers();
	}
	else {
		int first = 0;

		for (int i = 0; i < wallGroupIndex; i++)
			first += wallGroupsIndex[i];

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < level.entityManager->getEntitySize(); j++) {
				if (wallEntites[first] == level.entityManager->getEntity(j)) {
					level.entityManager->removeEntity(j);
					wallPoints.erase(wallPoints.begin() + first);
					scales.erase(scales.begin() + first);
					wallEntites.erase(wallEntites.begin() + first);;
				}
			}

		}

		wallGroupsIndex.erase(wallGroupsIndex.begin() + wallGroupIndex);
	}
}

void LevelStructure::addPoint(Level & level, int wallGroupIndex, unsigned pointIndex)
{
	int offset = 0;
	for (int i = 0; i < wallGroupIndex; i++) {
		offset += wallGroupsIndex[i];
	}

	int prev = (((pointIndex - offset) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;
	int next = (((pointIndex - offset + 1) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;

	Model* model = this->quad;

	Entity* entity = level.entityManager->addTracedEntity("WallPoint" + std::to_string(spawnedPoints));
	Transform* trans = entity->getTransform();

	glm::vec3 p1(wallPoints[prev].x - 1, 0.0, wallPoints[prev].z - 1);
	glm::vec3 p2(wallPoints[next].x, 0.0, wallPoints[next].z);

	glm::vec2 width = calcWallTransform(p1, p2, trans);

	entity->setModel(model);

	//Add wall collision
	this->addWallCollision(entity, level.collisionHandler);

	// Save scale
	this->scales.insert(scales.begin() + next, glm::vec2(glm::length(width), this->height));
	// Save upper wall points
	this->wallPoints.insert(wallPoints.begin() + next, { p1.x, this->height, p1.z });
	// Save entity pointer
	this->wallEntites.insert(wallEntites.begin() + next, entity);

	//Update transform for point before the newly added point
	entity = wallEntites[prev];
	trans = entity->getTransform();

	p2 = glm::vec3(wallPoints[prev].x, 0, wallPoints[prev].z);

	width = calcWallTransform(p2, p1, trans);

	// Update scale of next
	this->scales[prev] = glm::vec2(glm::length(width), this->height);

	wallGroupsIndex[wallGroupIndex] += 1;

	updateBuffers();
	spawnedPoints++;
}

void LevelStructure::editPoint(Level & level, int wallGroupIndex, int point, glm::vec3 newPoint)
{
	int offset = 0;
	for (int i = 0; i < wallGroupIndex; i++) {
		offset += wallGroupsIndex[i];
	}

	int prev = (((point - offset - 1) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;
	int next = (((point - offset + 1) + wallGroupsIndex[wallGroupIndex]) % wallGroupsIndex[wallGroupIndex]) + offset;

	Model* model = this->quad;
	Mesh* mesh = model->getMesh(0);

	Entity* entity = wallEntites[point];
	Transform* trans = entity->getTransform();

	glm::vec3 p1 = newPoint;
	glm::vec3 p2(wallPoints[next].x, 0.0, wallPoints[next].z);

	glm::vec3 width = p2 - p1;
	if (glm::length(width) > 0) {

		float angle = acosf(glm::dot(glm::normalize(width), { 1.0f, 0.0f, 0.0f }));
		if (glm::dot(glm::normalize(width), { 0.0f, 0.0f, -1.0f }) < 0.0f)
			angle = -angle;
		trans->setRotation(glm::vec3(angle, 0.0f, 0.0f));

		float dist = glm::length(width);
		glm::vec3 scale = glm::vec3(dist, this->height, 1.0f);
		trans->setScale(scale);
		trans->setPosition(p1);

		//Update the point after our new one
		entity = wallEntites[prev];
		trans = entity->getTransform();

		p2 = glm::vec3(wallPoints[prev].x, 0, wallPoints[prev].z);

		width = p1 - p2;
		if (glm::length(width) > 0.0f) {

			angle = acosf(glm::dot(glm::normalize(width), { 1.0f, 0.0f, 0.0f }));
			if (glm::dot(glm::normalize(width), { 0.0f, 0.0f, -1.0f }) < 0.0f)
				angle = -angle;
			trans->setRotation(glm::vec3(angle, 0.0f, 0.0f));

			dist = glm::length(width);
			scale = glm::vec3(dist, this->height, 1.0f);

			trans->setScale(scale);
			trans->setPosition(p2);

			wallPoints[point] = newPoint;
			updateBuffers();
		}
		else {
			LOG_ERROR("Wall Width is equal to 0, point to avoid this");
		}
	}
	else {
		LOG_ERROR("Wall Width is equal to 0, point to avoid this");
	}
}

std::vector<glm::vec3>& LevelStructure::getWallPoints()
{
	return this->wallPoints;
}

std::vector<int>& LevelStructure::getWallGroupsIndex()
{
	return this->wallGroupsIndex;
}

float LevelStructure::getWallHeight() const
{
	return this->height;
}

Model * LevelStructure::createQuad()
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
	Texture* tex = TextureManager::loadTexture("./Game/assets/textures/" + textureFile + ".png");
	mat.textures.push_back(tex);
	mat.Kd = glm::vec4(0.7f, 0.7f, 0.6f, 1.0f);
	mat.Ks_factor = glm::vec4(1.0f, 1.0f, 1.0f, 10.0f);
	quad->addMaterial(mat);

	ModelLoader::addModel("wall", quad);
	Display::get().getRenderer().addRenderingTarget(quad, SHADERS::WALL, true);

	return quad;
}

Model * LevelStructure::createInfPlane()
{
	Model* plane = new Model();

	std::vector<Vertex>* vertices = new	std::vector<Vertex>();
	std::vector<GLuint>* indicies = new std::vector<GLuint>();

	Vertex vertex;
	vertex.Normal = glm::vec3(0.0, 1.0, 0.0);

	vertex.Position = glm::vec3(-1.0, 0.0, -1.0);
	vertex.TexCoords = glm::vec2(0.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 0.0, -1.0);
	vertex.TexCoords = glm::vec2(1.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 0.0, 1.0);
	vertex.TexCoords = glm::vec2(1.0, 1.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(-1.0, 0.0, 1.0);
	vertex.TexCoords = glm::vec2(0.0, 1.0);
	vertices->push_back(vertex);

	indicies->push_back(0);
	indicies->push_back(3);
	indicies->push_back(1);
	indicies->push_back(1);
	indicies->push_back(3);
	indicies->push_back(2);

	Mesh* planeMesh = new Mesh(vertices, indicies, 0, plane);
	plane->addMesh(planeMesh);

	Material mat;
	float f = 0.5f;
	Texture* tex = TextureManager::loadTexture("./Game/assets/textures/wallTex.png");
	mat.textures.push_back(tex);
	mat.Kd = glm::vec4(0.7f, 0.7f, 0.6f, 1.0f);
	mat.Ks_factor = glm::vec4(1.0f, 1.0f , 1.0f, 10.0f);
	plane->addMaterial(mat);

	ModelLoader::addModel("infinityPlane", plane);
	Display::get().getRenderer().addRenderingTarget(plane, SHADERS::INFINITY_PLANE, false);

	return plane;
}

Model * LevelStructure::createRoofPlane()
{
	Model* plane = new Model();

	std::vector<Vertex>* vertices = new	std::vector<Vertex>();
	std::vector<GLuint>* indicies = new std::vector<GLuint>();

	Vertex vertex;
	vertex.Normal = glm::vec3(0.0, 1.0, 0.0);

	vertex.Position = glm::vec3(-1.0, 0.0, -1.0);
	vertex.TexCoords = glm::vec2(0.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 0.0, -1.0);
	vertex.TexCoords = glm::vec2(1.0, 0.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(1.0, 0.0, 1.0);
	vertex.TexCoords = glm::vec2(1.0, 1.0);
	vertices->push_back(vertex);

	vertex.Position = glm::vec3(-1.0, 0.0, 1.0);
	vertex.TexCoords = glm::vec2(0.0, 1.0);
	vertices->push_back(vertex);

	indicies->push_back(0);
	indicies->push_back(1);
	indicies->push_back(3);
	indicies->push_back(1);
	indicies->push_back(2);
	indicies->push_back(3);

	Mesh* planeMesh = new Mesh(vertices, indicies, 0, plane);
	plane->addMesh(planeMesh);

	Material mat;
	float f = 0.5f;
	Texture* tex = TextureManager::loadTexture("./Game/assets/textures/wallTex.png");
	mat.textures.push_back(tex);
	mat.Kd = glm::vec4(0.7f, 0.7f, 0.6f, 1.0f);
	mat.Ks_factor = glm::vec4(1.0f, 1.0f, 1.0f, 10.0f);
	plane->addMaterial(mat);

	ModelLoader::addModel("roof", plane);
	Display::get().getRenderer().addRenderingTarget(plane, SHADERS::ROOF_PLANE, false);

	return plane;
}

void LevelStructure::createWallBuffers()
{
	// Get all entites matrix in one vector
	std::vector<glm::mat4> mats;
	for (unsigned i = 0; i < this->wallEntites.size(); i++)
		mats.push_back(this->wallEntites[i]->getTransform()->getMatrix());

	Mesh* mesh = this->quad->getMesh(0);

	AttributeLayout matLayout;
	matLayout.push(4, 1);
	matLayout.push(4, 1);
	matLayout.push(4, 1);
	matLayout.push(4, 1);
	mesh->addBuffer(&mats[0][0], mats.size() * sizeof(glm::mat4), matLayout);

	AttributeLayout scaleLayout;
	scaleLayout.push(2, 1);
	mesh->addBuffer(&this->scales[0], this->scales.size() * sizeof(glm::vec2), scaleLayout);
}

void LevelStructure::updateBuffers()
{
	std::vector<glm::mat4> mats;
	for (unsigned i = 0; i < this->wallEntites.size(); i++) {
		mats.push_back(this->wallEntites[i]->getTransform()->getMatrix());
	}

	Mesh* mesh = this->quad->getMesh(0);

	mesh->updateInstancingData(&mats[0][0], mats.size() * sizeof(glm::mat4), 0, 3);
}

void LevelStructure::createInfinityPlane(Level& level)
{
	// Add an entity which has a model as a plane.
	Entity* entity = level.entityManager->addTracedEntity("InfinityPlane");
	entity->getTransform()->setScale(glm::vec3(100.f));
	entity->getTransform()->setPosition(glm::vec3(0.f, this->height, 0.f));
	entity->setModel(this->infPlane);
}

void LevelStructure::createRoof(Level & level)
{
	// Add an entity which has a model as a plane.
	Entity* entity = level.entityManager->addTracedEntity("Roof");
	entity->getTransform()->setScale(glm::vec3(100.f));
	entity->getTransform()->setPosition(glm::vec3(0.f, this->height, 0.f));
	entity->setModel(this->roofPlane);
}

bool LevelStructure::isClockwise(std::vector<glm::vec3>& points)
{
	glm::vec3* p0 = &points[0];
	glm::vec3* p1 = &points[(1) % (points.size())];
	glm::vec3* p2 = &points[(2) % (points.size())];

	glm::vec3 l1 = *p1 - *p0;
	glm::vec3 l2 = *p2 - *p0;

	glm::vec3 cross = glm::cross(l1, l2);

	if (cross.y > 0)
		return true;
	return true;
}

Utils::AABB LevelStructure::createBoundingBox(const glm::vec3 & e1, const glm::vec3 & e2, const glm::vec3 & e3)
{
	size_t size = this->wallPoints.size()*2;
	std::vector<glm::vec3> totalWallPoints(size);
	for (unsigned int i = 0; i < size; i+=2) {
		totalWallPoints[i] = this->wallPoints[i / 2];
		totalWallPoints[i+1] = this->wallPoints[i / 2]-glm::vec3(0.0f, this->height, 0.0f);
	}
	return Utils::getAABB(totalWallPoints, e1, e2, e3);
}
