#include "CollisionHandler.h"
#include "Engine/Entity/Entity.h"
#include "Utils/Logger.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "GLFW/glfw3.h"

#include "../Rendering/GLAbstraction/RenderingResources.h"

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

	// Create the shapes used by bodies
	createShapes();
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

	for (auto shapeVector : this->shapes)
		for (auto data : shapeVector) {
			delete data->shape;
			delete data;
		}

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
	else
		this->world->testCollision(&this->collision);
}

void CollisionHandler::createCollisionBodies(int num)
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

void CollisionHandler::addCollisionToEntity(Entity * entity, SHAPE shape)
{
	if (entity == nullptr)
	{
		LOG_WARNING("Nullptr as entity!");
		return;
	}

	rp3d::CollisionBody* entityBody = getUnusedBody();
	if (!entityBody)
	{
		LOG_WARNING("No empty collisionBodies!");
		return;
	}

	rp3d::Vector3 pos = toReactVec(entity->getTransform()->getPosition());
	rp3d::Quaternion rotation;
	glm::quat q = entity->getTransform()->getRotationQuat();
	rotation.setAllValues(q.x, q.y, q.z, q.w);
	rp3d::Transform transform(pos, rotation);

	/*
	SET SHAPE TO COLLISIONBODY. IF SHAPE IS ARROW/PLAYER SAVE THE POINTER FOR COLLISION CHECK
	*/

	if (shape == SHAPE::ARROW)
		this->player = entityBody;


	rp3d::Quaternion shapeRot = rp3d::Quaternion::identity();
#ifdef ENABLE_COLLISION_BOXES

	for (auto data : this->shapes[(size_t)shape]) {
		rp3d::ProxyShape* proxyShape = entityBody->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
		proxyShape->setUserData((void*)data);
		this->proxyShapes.push_back(proxyShape);
	}

#else

	for (auto data : this->shapes[(size_t)shape]) {
		entityBody->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
	}

#endif


	entityBody->setTransform(transform);

	// Set the body for the entity
	entity->setCollisionBody(entityBody);
	// Save the pair of entity and body for collision detection later
	this->entities[entityBody] = entity;
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

void CollisionHandler::addCollisionToEntity(Entity * entity, bool isPlayer)
{
	if (entity == nullptr)
	{
		LOG_WARNING("Nullptr as entity!");
		return;
	}

	if (entity->getModel() == nullptr)
	{
		LOG_WARNING("The entity's model is a nullptr!");
		return;
	}

	rp3d::CollisionBody* entityBody = getUnusedBody();
	if (!entityBody)
	{
		LOG_WARNING("No empty collisionBodies!");
		return;
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
	for (auto data : this->shapesMap[entity->getModel()]) {
		
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
				newData->color = { 1.0f, 0.f, 1.0f };
				newData->scale = entityScale;
				newData->pos = data->pos*entityScale;
				newData->size = data->size*entityScale;

				rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(newData->size));
				newData->shape = boxShape;

				map.shapes.push_back(newData);
			}
			else
			{
				newData = map.shapes[index];
			}

			// Use shape if exists.
			rp3d::ProxyShape* proxyShape = entityBody->addCollisionShape(newData->shape, rp3d::Transform(this->toReactVec(newData->pos), shapeRot));
			proxyShape->setUserData((void*)newData);
			this->proxyShapes.push_back(proxyShape);
		}
		else
		{
			rp3d::ProxyShape* proxyShape = entityBody->addCollisionShape(data->shape, rp3d::Transform(this->toReactVec(data->pos), shapeRot));
			proxyShape->setUserData((void*)data);
			this->proxyShapes.push_back(proxyShape);
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
}

void CollisionHandler::addShape(Model* modelPtr, Vertex* vertices, unsigned int numVertices)
{
	std::vector<Vertex> verts(vertices, vertices+numVertices);

	auto getAABB = [](Vertex* vertices, unsigned int numVertices)->std::pair<glm::vec3, glm::vec3> {
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

		return std::pair<glm::vec3, glm::vec3>(glm::max(max - min, 0.01f), min + (max-min)*0.5f);
	};

	std::pair<glm::vec3, glm::vec3> aabb = getAABB(vertices, numVertices);
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	data->color = { 1.0f, 1.0f, 1.0f };
	data->scale = { 1.0f, 1.0f, 1.0f };
	data->pos = aabb.second;
	data->size = aabb.first*0.5f;

	rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(data->size));
	data->shape = boxShape;

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

void CollisionHandler::createShapes()
{
	this->shapes.resize((size_t)SHAPE::SIZE);

	// Create and add shapes to the shapes vector. Order is important!

	
#ifdef ENABLE_COLLISION_BOXES
	//// DRONE = 0
	/*this->addShape(SHAPE::DRONE, { 0.25f, 0.25f, 0.25f }, { 0.15f, 0.5f, 0.15f });
	this->addShape(SHAPE::DRONE, { 0.05f, 0.05f, 0.05f }, { 0.75f, 0.15f, 0.15f }, { 0.0f, 0.05f, -0.25f });

	//// BOX = 1
	this->addShape(SHAPE::BOX, { 0.5f, 0.5f, 0.5f });

	//// ARROW = 2
	this->addShape(SHAPE::ARROW, { 0.05f, 0.05f, 0.5f }, { 0.0f, 0.0f, 1.0f });
	*/
#else
	//// DRONE = 0 ---- CHANGE TO A MESH WHEN DONE
	this->addShape(SHAPE::DRONE, { 0.25f, 0.25f, 0.25f }); 

	//// BOX = 1
	this->addShape(SHAPE::BOX, { 0.5f, 0.5f, 0.5f });

	//// ARROW = 2 ---- CHANGE TO A MESH WHEN DOWN
	this->addShape(SHAPE::ARROW, { 0.05f, 0.05f, 0.5f });
#endif
}



#ifdef ENABLE_COLLISION_BOXES

void CollisionHandler::toggleDrawing(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_F3 && ev->action == GLFW_PRESS) {
		this->drawCollisionShapes = !this->drawCollisionShapes;
	}
}


void CollisionHandler::addShape(SHAPE shape, const glm::vec3& scale, const glm::vec3& color, const glm::vec3& pos)
{
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	data->color = color;
	data->scale = scale;
	data->pos = pos;

	rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(data->scale));
	data->shape = boxShape;

	this->shapes[(size_t)shape].push_back(data);
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
	}

}

void CollisionHandler::drawCollisionBoxes()
{
	if(this->drawCollisionShapes)
		this->cRenderer.render();
}
#else


void CollisionHandler::addShape(SHAPE shape, const glm::vec3& scale, const glm::vec3 & pos)
{
	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	data->pos = pos;
	data->shape = new rp3d::BoxShape(this->toReactVec(scale));

	this->shapes[(size_t)shape].push_back(data);
}

#endif