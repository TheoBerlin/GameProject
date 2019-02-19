#include "CollisionHandler.h"
#include "Engine/Entity/Entity.h"
#include "Utils/Logger.h"


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
}


CollisionHandler::~CollisionHandler()
{
	// Delete world, deletes bodies and their proxyshapes aswell
	delete this->world;

	// Clear the bodies list of pointers
	this->bodies.clear();

	// Delete the shape pointers
	for (auto shape : this->shapes)
		delete shape;
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
	rp3d::Vector3 shapePos({ 0.0, 0.0, 0.0 });
	rp3d::Quaternion shapeRot = rp3d::Quaternion::identity();
	rp3d::Transform shapeTransform(shapePos, shapeRot);

	if (shape == SHAPE::ARROW) {
		this->player = entityBody;
		entityBody->addCollisionShape(this->shapes[(size_t)shape], shapeTransform)->setCollisionCategoryBits(CATEGORY::ARROW);
	}
	else if (shape == SHAPE::DRONE)
		entityBody->addCollisionShape(this->shapes[(size_t)shape], shapeTransform)->setCollisionCategoryBits(CATEGORY::DRONE_BODY);
	else
		entityBody->addCollisionShape(this->shapes[(size_t)shape], shapeTransform)->setCollisionCategoryBits(CATEGORY::STATIC);

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

rp3d::Vector3 CollisionHandler::toReactVec(const glm::vec3 & vec)
{
	return { vec.x, vec.y, vec.z };
}

glm::vec3 CollisionHandler::toGlmVec(const rp3d::Vector3 & vec)
{
	return { vec.x, vec.y, vec.z };
}

void CollisionHandler::createShapes()
{
	// Create and add shapes to the shapes vector. Order is important!

	// DRONE = 0 ---- CHANGE TO A MESH WHEN DONE
	rp3d::BoxShape* drone = new rp3d::BoxShape({ 0.25, 0.25, 0.25 });
	this->shapes.push_back(drone);

	// BOX = 1
	rp3d::BoxShape * box = new rp3d::BoxShape({ 0.25, 0.25, 0.25 });
	this->shapes.push_back(box);

	// ARROW = 2 ---- CHANGE TO A MESH WHEN DOWN
	rp3d::SphereShape* arrow = new rp3d::SphereShape(0.25);
	this->shapes.push_back(arrow);
}
