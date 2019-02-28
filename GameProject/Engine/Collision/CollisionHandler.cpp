#include "CollisionHandler.h"
#include "Engine/Entity/Entity.h"
#include "Utils/Logger.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "GLFW/glfw3.h"

#include "../Rendering/GLAbstraction/RenderingResources.h"
#include "Utils/Utils.h"

CollisionHandler::CollisionHandler()
{
	// Set default values
	this->takenBodies = 0;
	this->player = nullptr;

	// Give pointer to entites to collision
	this->collision.setEntitesPointer(&this->entities);

	// Set settings for world
	rp3d::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;

	// Create the world with the settings
	this->world = new rp3d::CollisionWorld(settings);

#ifdef ENABLE_COLLISION_BOXES
	//Register callback for drawing GUI
	EventBus::get().subscribe(this, &CollisionHandler::toggleDrawing);

	this->drawCollisionShapes = false;
#endif
}


CollisionHandler::~CollisionHandler()
{
	// Delete world, deletes bodies and their proxyshapes aswell
	delete this->world;

	// Clear the bodies list of pointers
	this->bodies.clear();

	/*for (auto shapeVector : this->shapes)
		for (auto data : shapeVector) {
			delete data->shape;
			delete data;
		}*/

	// Delete all the shapes
	for(auto& shapevector : this->shapesMap)
		for (auto data : shapevector.second) {
			delete data->shape;
			delete data;
		}

	for(auto& map : this->varientShapesMap)
		for (auto& data : map.second.shapes) {
			delete data->shape;
			delete data;
		}
}

void CollisionHandler::checkCollision()
{
	if (this->player)
		this->world->testCollision(this->player, &this->collision);
}

void CollisionHandler::createCollisionBodies(unsigned num)
{
	size_t previousSize = this->bodies.size();
	this->bodies.resize(this->bodies.size() + num);

	// Standard values for bodies which is not in use
	rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
	rp3d::Transform transform(initPosition, initOrientation);

	// Create the bodies in the world and add them to the map
	for (size_t i = previousSize; i < num; i++)
	{
		this->bodies[i] = this->world->createCollisionBody(transform);
	}
}

rp3d::CollisionBody * CollisionHandler::getUnusedBody()
{
	if (this->takenBodies == this->bodies.size())
		return nullptr;
	else
		return this->bodies[this->takenBodies++];
}

void CollisionHandler::removeCollisionBody(rp3d::CollisionBody * body)
{
	for (size_t i = 0; i < this->bodies.size(); i++)
	{
		if (this->bodies[i] == body)
		{
			this->world->destroyCollisionBody(body);
			this->bodies[i] = this->bodies[this->bodies.size() - 1];
			this->bodies.pop_back();
			this->takenBodies--;
		}
	}
}

void CollisionHandler::removeCollisionBody(Entity * entity)
{
	rp3d::CollisionBody* body = entity->getCollisionBody();
	
	rp3d::ProxyShape* current = body->getProxyShapesList();

	// Look for the proxy shape that contains the collision shape in parameter
	while (current != nullptr) {

		// Remove the proxy collision shape
		rp3d::ProxyShape* nextElement = current->getNext();
		
		// TEMPORARY
		rp3d::ProxyShape* proxyPtr;
		size_t size = this->proxyShapes.size();
		for (size_t i = 0; i < size; i++) {
			proxyPtr = this->proxyShapes[i];
			if (proxyPtr == current) {
				this->proxyShapes[i] = this->proxyShapes[size - 1];
				this->proxyShapes.pop_back();
			}
		}

		body->removeCollisionShape(current);

		// Get the next element in the list
		current = nextElement;
	}

	for (size_t i = 0; i < this->bodies.size(); i++)
	{
		if (this->bodies[i] == body)
		{
			rp3d::CollisionBody* temp = this->bodies[this->takenBodies - 1];
			this->bodies[this->takenBodies - 1] = body;
			this->bodies[i] = temp;
			this->takenBodies--;
			break;
		}
	}
}

std::vector<unsigned short*> CollisionHandler::addCollisionToEntity(Entity * entity, CATEGORY cat, bool isPlayer)
{
	std::vector<unsigned short*> categories;

	if (entity == nullptr)
	{
		LOG_WARNING("Nullptr as entity!");
		return categories;
	}

	if (entity->getModel() == nullptr)
	{
		LOG_WARNING("The entity's model is a nullptr!");
		return categories;
	}

	rp3d::CollisionBody* entityBody = getUnusedBody();
	if (!entityBody)
	{
		LOG_WARNING("No empty collisionBodies!");
		return categories;
	}

	rp3d::Vector3 pos = toReactVec(entity->getTransform()->getPosition());
	rp3d::Quaternion rotation;
	glm::quat q = entity->getTransform()->getRotationQuat();
	rotation.setAllValues(q.x, q.y, q.z, q.w);
	rp3d::Transform transform(pos, rotation);

	/*
	SET SHAPE TO COLLISIONBODY. IF SHAPE IS ARROW/PLAYER SAVE THE POINTER FOR COLLISION CHECK
	*/

	if (isPlayer)
		this->player = entityBody;

	rp3d::Quaternion shapeRot = rp3d::Quaternion::identity();
#ifdef ENABLE_COLLISION_BOXES
	bool addVarient = false;
	glm::vec3 entityScale = entity->getTransform()->getScale();
	unsigned int index = 0;
	for (auto data : this->shapesMap[entity->getModel()]) 
	{	
		if (data->scale != entity->getTransform()->getScale())
		{
			// Add shape for this scale if not exist.
			if (this->varientShapesMap.find(std::make_tuple(entityScale.x, entityScale.y, entityScale.z)) == this->varientShapesMap.end())
			{
				this->varientShapesMap.insert(std::pair<MapKey, MapData>(std::make_tuple(entityScale.x, entityScale.y, entityScale.z), MapData()));
				addVarient = true;
			}

			CollisionShapeDrawingData* newData = nullptr;
			MapData& map = this->varientShapesMap[std::make_tuple(entityScale.x, entityScale.y, entityScale.z)];

			// Add new shape if not exists.
			if (addVarient)
			{
				newData = new CollisionShapeDrawingData();
				constructShape(newData, data->pos*entityScale, data->size*entityScale, cat, entityScale, {1.f, 0.f, 1.f});
				map.shapes.push_back(newData);
			}
			else
			{
				newData = map.shapes[index];
			}

			// Use shape if exists.
			addCollisionShapeToBody(entityBody, newData, shapeRot);
			categories.push_back(&newData->category);
		}
		else
		{
			addCollisionShapeToBody(entityBody, data, shapeRot);
			categories.push_back(&data->category);
		}
		index++;
	}

#else

	for (auto data : this->shapesMap[modelPtr]) {
		entityBody->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
	}

#endif

	entityBody->setTransform(transform);

	// Set the body for the entity
	entity->setCollisionBody(entityBody);
	// Save the pair of entity and body for collision detection later
	this->entities[entityBody] = entity;

	return categories;
}

void CollisionHandler::addShape(Model* modelPtr, Vertex* vertices, unsigned int numVertices)
{
	getOOBB(vertices, numVertices);
	std::pair<glm::vec3, glm::vec3> aabb = getAABB(vertices, numVertices);
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	constructShape(data, aabb.second, aabb.first*.5f);
	this->shapesMap[modelPtr].push_back(data);
}

rp3d::Vector3 CollisionHandler::toReactVec(const glm::vec3 & vec)
{
	return { vec.x, vec.y, vec.z };
}

glm::vec3 CollisionHandler::toGlmVec(const rp3d::Vector3 & vec)
{
	return { vec.x, vec.y, vec.z };
}

glm::quat CollisionHandler::toGlmQuat(const rp3d::Quaternion & vec)
{
	glm::quat q;
	q.x = vec.x;
	q.y = vec.y;
	q.z = vec.z;
	q.w = vec.w;

	return q;
}

std::pair<glm::vec3, glm::vec3> CollisionHandler::getAABB(Vertex * vertices, unsigned int numVertices)
{
	glm::vec3 min = glm::vec3(10000.0f);
	glm::vec3 max = glm::vec3(-10000.0f);
	for (unsigned int i = 0; i < numVertices; i++)
	{
		Vertex& vert = vertices[i];
		min.x = glm::min(min.x, vert.Position.x);
		min.y = glm::min(min.y, vert.Position.y);
		min.z = glm::min(min.z, vert.Position.z);

		max.x = glm::max(max.x, vert.Position.x);
		max.y = glm::max(max.y, vert.Position.y);
		max.z = glm::max(max.z, vert.Position.z);
	}

	return std::pair<glm::vec3, glm::vec3>(glm::max(max - min, 0.01f), min + (max - min)*0.5f);
}

// TODO: Change return type to tuple with position, size and rotation.
std::pair<glm::vec3, glm::vec3> CollisionHandler::getOOBB(Vertex * vertices, unsigned int numVertices)
{
	std::vector<Vertex> verts(vertices, vertices + numVertices);

	// Calculate centroid.
	glm::vec3 centroid(0.0f);
	for (Vertex& v : verts)
		centroid += v.Position;
	centroid /= numVertices;

	// Distance from centroid to vertex.
	std::vector<glm::vec3> changeInPos;
	for (Vertex& v : verts)
		changeInPos.push_back(v.Position - centroid);

	/* Matrix multiplication for a single element.
		c[i][j] = (a*b)[i][j]
	Arguments:
		m1: matrix a
		m2: matrix b
		i: row index
		j: col index
	*/
	auto getElem = [](std::vector<glm::vec3>& m1, std::vector<glm::vec3>& m2, int i, int j)->float {
		float a = 0.0f;
		for (unsigned k = 0; k < m1.size(); k++)
			a += m1[k][i] * m2[k][j];
		return a;
	};

	// Calculate the covariance matrix
	float cov[3][3];
	for (unsigned i = 0; i < 3; i++)
		for (unsigned j = 0; j < 3; j++)
			cov[i][j] = getElem(changeInPos, changeInPos, i, j);
	
	/*
	// This might be needed.
	for (unsigned i = 0; i < 3; i++)
		for (unsigned j = 0; j < 3; j++)
			cov[i][j] /= 3.f;
			*/

	// Find characteristic equation det(A-l*I) = 0
	double a = -(double)(cov[0][0] + cov[1][1] + cov[2][2]);
	double b = -(double)(-cov[0][0] * cov[1][1] - cov[0][0] * cov[2][2] - cov[1][1] * cov[2][2] + cov[1][2] * cov[2][1] + cov[0][1] * cov[1][0] + cov[0][2] * cov[2][0]);
	double c = -(double)(cov[0][0] * cov[1][1] * cov[2][2] - cov[0][0] * cov[1][2] * cov[2][1] - cov[0][1] * cov[1][0] * cov[2][2] + cov[0][1] * cov[1][2] * cov[2][0] + cov[0][2] * cov[1][0] * cov[2][1] - cov[0][2] * cov[1][1] * cov[2][0]);

	// Solve the cubic equation.
	Utils::CubicResult cubicResult = Utils::solveCubic(a, b, c);

	// For finding eigenvectors: http://wwwf.imperial.ac.uk/metric/metric_public/matrices/eigenvalues_and_eigenvectors/eigenvalues2.html

	glm::mat3 mat({ cov[0][0], cov[0][1], cov[0][2] }, { cov[1][0], cov[1][1], cov[1][2] }, { cov[2][0], cov[2][1], cov[2][2] });

	if (cubicResult.towEqual)
	{
		glm::vec3 e = Utils::calcEigenvector(cubicResult.x1_real, mat);
	}
	else
	{
		glm::vec3 e1 = Utils::calcEigenvector(cubicResult.x1_real, mat);
		glm::vec3 e2 = Utils::calcEigenvector(cubicResult.x2_real, mat);
		glm::vec3 e3 = Utils::calcEigenvector(cubicResult.x3_real, mat);

		//this->lines.push_back(std::pair<glm::vec3, glm::vec3>(centroid, centroid + glm::normalize(e1)));
		//this->lines.push_back(std::pair<glm::vec3, glm::vec3>(centroid, centroid + glm::normalize(e2)));
		//this->lines.push_back(std::pair<glm::vec3, glm::vec3>(centroid, centroid + glm::normalize(e3)));
	}

	if(lines.empty())
		this->lines.push_back(std::pair<glm::vec3, glm::vec3>({ 0.f, 0.f, 0.f }, {0.f, 6.f, 0.f}));

	//glm::mat3 mat3({ -2.f, -4.f, 2.f }, { -2.f, 1.f, 2.f }, { 4.f, 2.f, 5.f }); // eigenValues: 3, -5, 6
	//glm::mat3 mat3({ 3.f, 2.f, 4.f }, { 2.f, 0.f, 2.f }, { 4.f, 2.f, 3.f }); // eigenValues: -1, -1, 8
	
	//glm::vec3 e = Utils::calcEigenvector(-1, mat3);

	return std::pair<glm::vec3, glm::vec3>();
}

void CollisionHandler::constructShape(CollisionShapeDrawingData* data, const glm::vec3 & pos, const glm::vec3 & size, CATEGORY cat, const glm::vec3& scale, const glm::vec3& color)
{
	data->color = color;
	data->scale = scale;
	data->pos = pos;
	data->size = size;
	data->category = cat;

	rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(data->size));
	data->shape = boxShape;
}

void CollisionHandler::addCollisionShapeToBody(rp3d::CollisionBody * body, CollisionShapeDrawingData * data, const rp3d::Quaternion& shapeRot)
{
	rp3d::ProxyShape* proxyShape = body->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
	proxyShape->setUserData((void*)data);
	proxyShape->setCollisionCategoryBits(data->category);
	this->proxyShapes.push_back(proxyShape);
}



#ifdef ENABLE_COLLISION_BOXES

void CollisionHandler::toggleDrawing(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_F3 && ev->action == GLFW_PRESS) {
		this->drawCollisionShapes = !this->drawCollisionShapes;
	}
}

void CollisionHandler::updateDrawingData()
{
	if (this->drawCollisionShapes) {
		this->matrices.clear();
		this->colors.clear();

		for (auto proxyShape : this->proxyShapes) {
			CollisionShapeDrawingData* data = (CollisionShapeDrawingData*)(proxyShape->getUserData());
			rp3d::Transform trans = proxyShape->getLocalToWorldTransform();
			glm::vec3 eulerAngles = glm::eulerAngles(this->toGlmQuat(trans.getOrientation()));

			glm::mat4 mat(1.0f);
			mat = glm::translate(mat, this->toGlmVec(trans.getPosition()));
			mat = glm::rotate(mat, eulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
			mat = glm::rotate(mat, eulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
			mat = glm::rotate(mat, eulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
			mat = glm::scale(mat, data->size);
			this->matrices.push_back(mat);
			this->colors.push_back(data->color);
		}

		this->cRenderer.updateColors(colors);
		this->cRenderer.updateMatrices(matrices);


		// Update lines
		this->lines2.clear();
		this->matricesLines.clear();
		this->colorsLines.clear();
		for (std::pair<glm::vec3, glm::vec3>& line : this->lines) {
			glm::mat4 mat(1.0f);
			this->lines2.push_back(line.first);
			this->lines2.push_back(line.second);
			this->matricesLines.push_back(mat);
			this->colorsLines.push_back({1.f, 1.f, 1.f});
		}

		this->cRenderer.updateLines(this->lines2);
		this->cRenderer.updateColorsLine(colorsLines);
		this->cRenderer.updateMatricesLine(matricesLines);
	}
}

void CollisionHandler::drawCollisionBoxes()
{
	if (this->drawCollisionShapes)
		this->cRenderer.render();
}
#else


void CollisionHandler::addShape(SHAPE shape, CATEGORY cat, const glm::vec3& scale, const glm::vec3 & pos)
{
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	data->pos = pos;
	data->shape = new rp3d::BoxShape(this->toReactVec(scale));
	data->category = cat;

	this->shapes[(size_t)shape].push_back(data);
}

#endif