#include "CollisionHandler.h"
#include "Engine/Entity/Entity.h"
#include "Utils/Logger.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "GLFW/glfw3.h"

#include "../Rendering/GLAbstraction/RenderingResources.h"
#include <cmath>
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
				constructShape(newData, data->pos*entityScale, data->size*entityScale, data->rot, cat, entityScale, {1.f, 0.f, 1.f});
				map.shapes.push_back(newData);
			}
			else
			{
				newData = map.shapes[index];
			}

			// Use shape if exists.
			addCollisionShapeToBody(entityBody, newData);
			categories.push_back(&newData->category);
		}
		else
		{
			addCollisionShapeToBody(entityBody, data);
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
	//AABB aabb = getAABB(vertices, numVertices);
	//OBB obb(aabb.first, aabb.second, glm::quat(1.f, 0.f, 0.f, 0.f));

	OBB obb = getOBB(modelPtr, vertices, numVertices);
	if (std::get<0>(obb).x == 0 && std::get<0>(obb).y == 0 && std::get<0>(obb).z == 0) return;
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	glm::vec3 size = std::get<0>(obb)*.5f;
	size.x = glm::max(size.x, 0.01f);
	size.y = glm::max(size.y, 0.01f);
	size.z = glm::max(size.z, 0.01f);
	constructShape(data, std::get<1>(obb), size, std::get<2>(obb));
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

CollisionHandler::AABB CollisionHandler::getAABB(Vertex * vertices, unsigned int numVertices, glm::vec3 e1, glm::vec3 e2, glm::vec3 e3)
{
	glm::vec3 min = glm::vec3(10000.0f);
	glm::vec3 max = glm::vec3(-10000.0f);
	for (unsigned int i = 0; i < numVertices; i++)
	{
		Vertex& vert = vertices[i];
		float e1dot = glm::dot(e1, vert.Position);
		float e2dot = glm::dot(e2, vert.Position);
		float e3dot = glm::dot(e3, vert.Position);
		min.x = glm::min(min.x, e1dot);
		min.y = glm::min(min.y, e2dot);
		min.z = glm::min(min.z, e3dot);

		max.x = glm::max(max.x, e1dot);
		max.y = glm::max(max.y, e2dot);
		max.z = glm::max(max.z, e3dot);
	}

	glm::vec3 pos = e1 * (min.x + (max.x - min.x)*.5f) + e2 * (min.y + (max.y - min.y)*.5f) + e3 * (min.z + (max.z - min.z)*.5f);

	return AABB(glm::max(max - min, 0.01f), pos);
}

CollisionHandler::OBB CollisionHandler::getOBB(Model* modelPtr, Vertex * vertices, unsigned int numVertices)
{
	std::vector<Vertex> verts(vertices, vertices + numVertices);

	// Calculate centroid.
	glm::vec3 centroid(0.0f);
	for (Vertex& v : verts)
		centroid += v.Position;
	centroid /= (float)numVertices;

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

	// Calculate the covariance matrix. By multiplying the Nx3 matrix with its transpose from the left we get a 3x3 matrix which will be the covariance matrix after normalization. 
	glm::mat3 cov;
	for (unsigned i = 0; i < 3; i++)
		for (unsigned j = 0; j < 3; j++)
			cov[j][i] = getElem(changeInPos, changeInPos, i, j) / 3.f; // Multiply by its transpose and divide by the dimension size to normalize it.

	// Calculate the eigenvectors from the covariance matrix. This will also give us the eigenvalues as a byproduct but we will not use them.
	std::pair<glm::vec3, glm::mat3> jacobiResult = Utils::jacobiMethod(cov);

	glm::vec3 e1 = glm::normalize(jacobiResult.second[0]);
	glm::vec3 e2 = glm::normalize(jacobiResult.second[1]);
	glm::vec3 e3 = glm::normalize(jacobiResult.second[2]);
	/*
	this->lines[modelPtr].push_back(std::tuple<glm::vec3, glm::vec3, glm::vec3>(centroid, centroid + e1, glm::vec3{ 1.f, 0.f, 0.f }));
	this->lines[modelPtr].push_back(std::tuple<glm::vec3, glm::vec3, glm::vec3>(centroid, centroid + e2, glm::vec3{ 0.f, 1.f, 0.f }));
	this->lines[modelPtr].push_back(std::tuple<glm::vec3, glm::vec3, glm::vec3>(centroid, centroid + e3, glm::vec3{ 0.f, 0.f, 1.f }));
	*/

	// Get the AABB in the base [e1 e2 e3].
	AABB aabb = getAABB(vertices, numVertices, e1, e2, e3);

	/*
	Return a quaternion to describe the rotation from v1 to v2.
	*/
	auto rotateTo = [](const glm::vec3& v1, const glm::vec3& v2)->glm::quat {
		glm::quat q;
		glm::vec3 n = glm::normalize(glm::cross(v1, v2));
		float angle = acosf(glm::dot(v1, v2));
		q.x = n.x*sinf(angle / 2.f);
		q.y = n.y*sinf(angle / 2.f);
		q.z = n.z*sinf(angle / 2.f);
		q.w = cosf(angle / 2.f);
		return q;
	};

	// Rotate the x-axis to the first eigenvector.
	glm::vec3 vx = glm::vec3{ 1.f, 0.f, 0.f };
	glm::quat rotX = rotateTo(vx, e1);

	// If the first eigenvector is parallel to the x-axis, do not rotate.
	float d = glm::dot(vx, e1);
	if (abs(d) > 0.999f)
		rotX = glm::quat(1.f, 0.f, 0.f, 0.f);
		
	// Rotate the z-axis by the previous rotation and then rotate that vector to the second eigenvector.
	// This is equivalent to make a roll around the previously calculated axis to match the second and third eigenvector..
	glm::vec3 vz = glm::normalize(rotX * glm::vec3(0.f, 0.f, 1.f));
	glm::quat roll = rotateTo(vz, e3);

	// If the second eigenvector is parallel to the new z-axis, do not rotate.
	d = glm::dot(e3, vz);
	if (abs(d) > 0.999f)
		roll = glm::quat(1.f, 0.f, 0.f, 0.f);

	return OBB(aabb.first, aabb.second, roll * rotX);
		
}

void CollisionHandler::constructShape(CollisionShapeDrawingData* data, const glm::vec3 & pos, const glm::vec3 & size, glm::quat rot, CATEGORY cat, const glm::vec3& scale, const glm::vec3& color)
{
	data->color = color;
	data->scale = scale;
	data->pos = pos;
	data->size = size;
	data->category = cat;
	data->rot = rot;

	rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(data->size));
	data->shape = boxShape;
}

void CollisionHandler::addCollisionShapeToBody(rp3d::CollisionBody * body, CollisionShapeDrawingData * data)
{
	rp3d::Quaternion shapeRot;
	shapeRot.setAllValues(data->rot.x, data->rot.y, data->rot.z, data->rot.w);
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
		if (this->lines.empty() == false)
		{
			this->lines2.clear();
			this->matricesLines.clear();
			this->colorsLines.clear();
			for (auto& lineArr : this->lines) {
				glm::mat4 mat(1.0f);
				for (std::tuple<glm::vec3, glm::vec3, glm::vec3>& line : lineArr.second)
				{
					this->lines2.push_back(std::get<0>(line));
					this->lines2.push_back(std::get<1>(line));
					this->matricesLines.push_back(mat);
					this->colorsLines.push_back(std::get<2>(line));
					this->colorsLines.push_back(std::get<2>(line));
				}
			}

			this->cRenderer.updateLines(this->lines2);
			this->cRenderer.updateColorsLine(colorsLines);
			this->cRenderer.updateMatricesLine(matricesLines);
		}
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