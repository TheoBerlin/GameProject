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

#ifdef ENABLE_COLLISION_DEBUG_DRAW
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

#ifdef ENABLE_COLLISION_DEBUG_DRAW
		rp3d::ProxyShape* proxyPtr;
		size_t size = this->proxyShapes.size();
		for (size_t i = 0; i < size; i++) {
			proxyPtr = this->proxyShapes[i];
			if (proxyPtr == current) {
				this->proxyShapes[i] = this->proxyShapes[size - 1];
				this->proxyShapes.pop_back();
			}
		}
#endif

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

void CollisionHandler::addCollisionToEntity(Entity * entity, CATEGORY cat, bool isPlayer, glm::quat rot, int desiredIndex)
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

	rp3d::Transform transform = getTransformFromEntity(entity);

	// If shape is player (Arrow), save the pointer for collision check.
	if (isPlayer)
		this->player = entityBody;


	unsigned int index = 0;
	glm::vec3 entityScale = entity->getTransform()->getScale();
	for (auto data : this->shapesMap[entity->getModel()]) 
	{
		if((unsigned)desiredIndex == index || desiredIndex == -1)
			addVariedCollisionShapeToBody(index, data, entityBody, cat, entityScale, glm::vec3(1.f), glm::vec3(0.f), rot);

		index++;
	}

	entityBody->setTransform(transform);

	// Set the body for the entity
	entity->setCollisionBody(entityBody);

	// Save the pair of entity and body for collision detection later
	this->entities[entityBody] = entity;
}

void CollisionHandler::addCollisionToEntity(Entity * entity, const std::vector<ShapeData>& shapeData, bool isPlayer)
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

	if (shapeData.empty())
	{
		LOG_WARNING("There are no data for the collision shape!");
		return;
	}

	rp3d::CollisionBody* entityBody = getUnusedBody();
	if (!entityBody)
	{
		LOG_WARNING("No empty collisionBodies!");
		return;
	}

	rp3d::Transform transform = getTransformFromEntity(entity);

	// If shape is player (Arrow), save the pointer for collision check.
	if (isPlayer)
		this->player = entityBody;


	unsigned int numShapes = this->shapesMap[entity->getModel()].size();
	
	// List of indices which corresponds to data in the shapeData list. -1 means it has not been chosen yet.
	std::vector<int> orderedDataExist(numShapes, -1);

	std::vector<std::string> orderedDataNames;
	for (auto data : this->shapesMap[entity->getModel()])
		orderedDataNames.push_back(data->name);

	// Put the right shapeData on the appropriate index.
	std::vector<ShapeData> orderedData(numShapes);
	for (unsigned int i = 0; i < orderedData.size(); i++) {
		for (unsigned int j = 0; j < shapeData.size(); j++) {
			ShapeData d = shapeData[j];
			// If not chosen, pick a shape with index of -1.
			if (orderedDataExist[i] < 0 && (d.index == -1 && d.name.empty())) {
				orderedData[i] = d;
			}

			// If not chosen and index of shapeData is the same, pick this.
			if (i == j && orderedDataExist[i] < 0 && (d.index == -1 || d.index == i)) {
				orderedData[i] = d;
				orderedDataExist[i] = j;
			}

			// If this has the correct index or name. Pick this and go to next ordered element.
			if (i == d.index || (d.name == orderedDataNames[i] && d.name.empty() == false)) {
				orderedData[i] = d;
				orderedDataExist[i] = j;
				break;
			}
		}
	}

	unsigned int index = 0;
	for (auto data : this->shapesMap[entity->getModel()])
	{
		ShapeData shape = orderedData[index];
		addVariedCollisionShapeToBody(index++, data, entityBody, shape.category, shape.scale, shape.localScale, shape.offset, shape.rotation);
	}

	entityBody->setTransform(transform);

	// Set the body for the entity
	entity->setCollisionBody(entityBody);
	// Save the pair of entity and body for collision detection later
	this->entities[entityBody] = entity;
}

void CollisionHandler::constructBoundingBox(Model* modelPtr, Vertex* vertices, unsigned int numVertices, const std::string& name, bool makeAABB)
{
	Utils::OBB boundingBox;
	if (makeAABB) {
		Utils::AABB aabb = Utils::getAABB(vertices, numVertices);
		boundingBox = { aabb.size, aabb.pos, glm::quat(1.f, 0.f, 0.f, 0.f) };
	}
	else {
		boundingBox = Utils::getOBB(vertices, numVertices);
	}

	// Check if it failed to create the bounding box.
	if (boundingBox.size.x == 0 && boundingBox.size.y == 0 && boundingBox.size.z == 0) return;

	CollisionShapeDrawingData* data = new CollisionShapeDrawingData();
	glm::vec3 scale = boundingBox.size*.5f;
	scale.x = glm::max(scale.x, 0.01f);
	scale.y = glm::max(scale.y, 0.01f);
	scale.z = glm::max(scale.z, 0.01f);
	constructShape(data, boundingBox.pos, scale, boundingBox.rot);
	data->name = name;
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

void CollisionHandler::constructShape(CollisionShapeDrawingData* data, const glm::vec3 & pos, const glm::vec3 & size, glm::quat rot, CATEGORY cat, const glm::vec3& scale, const glm::vec3& color)
{
#ifdef ENABLE_COLLISION_BOXES
	data->color = color;
#endif
	data->scale = scale;
	data->pos = pos;
	data->size = size;
	data->category = cat;
	data->rot = rot;

	rp3d::BoxShape * boxShape = new rp3d::BoxShape(toReactVec(data->size));
	data->shape = boxShape;
}

void CollisionHandler::addCollisionShapeToBody(rp3d::CollisionBody * body, rp3d::CollisionShape* shape, CATEGORY category, const glm::vec3& pos, const glm::quat& rotation, CollisionShapeDrawingData* data)
{
	rp3d::Quaternion shapeRot;
	shapeRot.setAllValues(rotation.x, rotation.y, rotation.z, rotation.w);
	rp3d::ProxyShape* proxyShape = body->addCollisionShape(shape, rp3d::Transform(this->toReactVec(pos), shapeRot));
	proxyShape->setCollisionCategoryBits(category);
	if(data != nullptr)
		proxyShape->setUserData((void*)data);
#ifdef ENABLE_COLLISION_DEBUG_DRAW
	this->proxyShapes.push_back(proxyShape);
#endif
}

void CollisionHandler::addVariedCollisionShapeToBody(unsigned int index, CollisionShapeDrawingData* shape, rp3d::CollisionBody* body, CATEGORY category, const glm::vec3& scale, const glm::vec3& localScale, const glm::vec3& offset, const glm::quat& rotation)
{
	bool addVarient = false;
	// Construct new shape if scale does not match.
	glm::vec3 totalScale = scale * localScale;
	if (shape->scale != scale)
	{
		// Add shape for this scale if it does not exist.
		if (this->varientShapesMap.find(std::make_tuple(totalScale.x, totalScale.y, totalScale.z)) == this->varientShapesMap.end())
		{
			this->varientShapesMap.insert(std::pair<MapKey, MapData>(std::make_tuple(totalScale.x, totalScale.y, totalScale.z), MapData()));
			addVarient = true;
		}

		CollisionShapeDrawingData* newShape = nullptr;
		MapData& map = this->varientShapesMap[std::make_tuple(totalScale.x, totalScale.y, totalScale.z)];

		// Add new shape if it does not exist.
		if(map.shapes.size()-1 < index || addVarient)
		{
			newShape = new CollisionShapeDrawingData();
			constructShape(newShape, shape->pos*scale, shape->size*totalScale, shape->rot, category, totalScale, { 1.f, 0.f, 1.f });
			map.shapes.push_back(newShape);
		}
		else
		{
			// Use shape if exists.
			newShape = map.shapes[index];
		}

		addCollisionShapeToBody(body, newShape->shape, (CATEGORY)(newShape->category != (unsigned short)category ? category : newShape->category), newShape->pos + offset, rotation * newShape->rot, newShape);
	}
	else
	{
		addCollisionShapeToBody(body, shape->shape, (CATEGORY)(shape->category != (unsigned short)category ? category : shape->category), shape->pos + offset, rotation * shape->rot, shape);
	}
}

rp3d::Transform CollisionHandler::getTransformFromEntity(Entity * entity)
{
	rp3d::Vector3 pos = toReactVec(entity->getTransform()->getPosition());
	rp3d::Quaternion rotation;
	glm::quat q = entity->getTransform()->getRotationQuat();
	rotation.setAllValues(q.x, q.y, q.z, q.w);
	return rp3d::Transform(pos, rotation);
}

#ifdef ENABLE_COLLISION_DEBUG_DRAW

void CollisionHandler::toggleDrawing(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_F3 && ev->action == GLFW_PRESS) {
		this->drawCollisionShapes = !this->drawCollisionShapes;
	}
}

void CollisionHandler::updateDrawingData()
{
	if (this->drawCollisionShapes) {
	#if defined(ENABLE_COLLISION_BOXES)
		this->matrices.clear();
		this->colors.clear();
	#endif

	#if defined(ENABLE_AXIS_FOR_COLLISION_BOXES)
		this->lines.clear();
	#endif

		for (auto proxyShape : this->proxyShapes) {
			CollisionShapeDrawingData* data = (CollisionShapeDrawingData*)(proxyShape->getUserData());
			rp3d::Transform trans = proxyShape->getLocalToWorldTransform();
			glm::quat rot = this->toGlmQuat(trans.getOrientation());
			glm::vec3 pos = this->toGlmVec(trans.getPosition());

	#if defined(ENABLE_COLLISION_BOXES)
			glm::vec3 eulerAngles = glm::eulerAngles(rot);

			glm::mat4 mat(1.0f);
			mat = glm::translate(mat, pos);
			mat = glm::rotate(mat, eulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
			mat = glm::rotate(mat, eulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
			mat = glm::rotate(mat, eulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
			mat = glm::scale(mat, data->size);
			this->matrices.push_back(mat);
			this->colors.push_back(data->color);
	#endif

	#if defined(ENABLE_AXIS_FOR_COLLISION_BOXES)
			glm::vec3 e1 = rot * glm::vec3(1.f, 0.f, 0.f) * 0.2f;
			glm::vec3 e2 = rot * glm::vec3(0.f, 1.f, 0.f) * 0.2f;
			glm::vec3 e3 = rot * glm::vec3(0.f, 0.f, 1.f) * 0.2f;

			this->lines.push_back(std::tuple<glm::vec3, glm::vec3, glm::vec3>(pos, pos + e1, glm::vec3{ 1.f, 0.f, 0.f }));
			this->lines.push_back(std::tuple<glm::vec3, glm::vec3, glm::vec3>(pos, pos + e2, glm::vec3{ 0.f, 1.f, 0.f }));
			this->lines.push_back(std::tuple<glm::vec3, glm::vec3, glm::vec3>(pos, pos + e3, glm::vec3{ 0.f, 0.f, 1.f }));
	#endif
		}

	#if defined(ENABLE_COLLISION_BOXES)
		this->cRenderer.updateColors(colors);
		this->cRenderer.updateMatrices(matrices);
	#endif

	#if defined(ENABLE_AXIS_FOR_COLLISION_BOXES)
		// Update lines
		if (this->lines.empty() == false)
		{
			this->linesDivided.clear();
			this->matricesLines.clear();
			this->colorsLines.clear();
			glm::mat4 mat(1.0f);
			for (std::tuple<glm::vec3, glm::vec3, glm::vec3>& line : this->lines)
			{
				this->linesDivided.push_back(std::get<0>(line));
				this->linesDivided.push_back(std::get<1>(line));
				this->matricesLines.push_back(mat);
				this->colorsLines.push_back(std::get<2>(line));
				this->colorsLines.push_back(std::get<2>(line));
			}

			this->cRenderer.updateLines(this->linesDivided);
			this->cRenderer.updateColorsLine(colorsLines);
			this->cRenderer.updateMatricesLine(matricesLines);
		}
	#endif
	}
}
void CollisionHandler::drawCollisionBoxes()
{
	if (this->drawCollisionShapes)
		this->cRenderer.render();
}
#endif