#include "LevelStructure.h"

#include "Engine/AssetManagement/Model.h"
#include <Engine/AssetManagement/Mesh.h>
#include <Engine/Rendering/GLAbstraction/RenderingResources.h>
#include <Engine/AssetManagement/TextureManager.h>
#include <Engine/AssetManagement/ModelLoader.h>
#include <Game/Level/Level.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Entity/Transform.h>

LevelStructure::LevelStructure()
{
	this->quad = createQuad();
	this->plane = createPlane();
}


LevelStructure::~LevelStructure()
{
}

void LevelStructure::createWalls(Level & level, std::vector<glm::vec3>& points, float height)
{
	std::vector<glm::vec2> scales;

	Model* model = this->quad;
	Mesh* mesh = model->getMesh(0);

	std::vector<glm::mat4> mats;
	for (int i = 0; i < points.size(); i++)
	{
		Entity* entity = level.entityManager->addTracedEntity("WallPoint" + std::to_string(i));
		Transform* trans = entity->getTransform();
		glm::vec3* p1 = &points[i];
		glm::vec3* p2 = &points[(i + 1) % (points.size())];

		glm::vec3 width = *p2 - *p1;

		float angle = acosf(glm::dot(glm::normalize(width), { 1.0f, 0.0f, 0.0f }));
		if (glm::dot(glm::normalize(width), { 0.0f, 0.0f, -1.0f }) < 0.0f)
			angle = -angle;
		trans->setRotation(glm::vec3(0.0f, angle, 0.0f));

		float dist = glm::length(width);
		glm::vec3 scale = glm::vec3(dist, height, 1.0f);
		trans->setScale(scale);

		trans->setPosition(*p1);

		mats.push_back(trans->getMatrix());
		entity->setModel(model);

		scales.push_back(glm::vec2(glm::length(width), height));

		std::vector<Vertex> vertex(mesh->getVerticies().size());
		unsigned index = 0;
		for (Vertex& v : mesh->getVerticies())
		{
			vertex[index++].Position = scale * v.Position;
		}

		// Save upper wall points
		glm::vec4 u1((*p1).x, 1.f, (*p1).z, 1.f);
		u1 = trans->getMatrix() * u1;
		this->wallPoints.push_back(glm::vec3(u1.x, height, u1.z));


		// Add collision
		level.collisionHandler->constructBoundingBox(model, &vertex[0], vertex.size(), "");
		trans->setScale(glm::vec3(1.f, 1.f, 1.f));
		level.collisionHandler->addCollisionToEntity(entity, CATEGORY::STATIC, false, glm::quat(1.f, 0.f, 0.f, 0.f), i);
		trans->setScale(scale);
	}

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

	// Add an entity which has a model as a plane.
	Entity* entity = level.entityManager->addTracedEntity("InfinityPlane");
	entity->getTransform()->setScale(glm::vec3(100.f));
	entity->getTransform()->setPosition(glm::vec3(0.f, height, 0.f));
	entity->setModel(this->plane);
	ModelLoader::addModel("infinityPlane", this->plane);
}

std::vector<glm::vec3>& LevelStructure::getWallPoints()
{
	return this->wallPoints;
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
	Texture* tex = TextureManager::loadTexture("./Game/assets/textures/wallTex.png");
	mat.textures.push_back(tex);
	mat.Ks_factor = glm::vec4(40.0f);
	quad->addMaterial(mat);

	return quad;
}

Model * LevelStructure::createPlane()
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
	mat.Ks_factor = glm::vec4(40.0f);
	plane->addMaterial(mat);

	return plane;
}
