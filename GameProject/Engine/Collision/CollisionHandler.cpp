#include "CollisionHandler.h"
#include "Engine/Entity/Entity.h"
#include "Utils/Logger.h"


CollisionHandler::CollisionHandler()
{
	// Set default values
	this->takenBodies = 0;
	this->player = nullptr;

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

void CollisionHandler::update(const float & dt)
{
	if (player)
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
		LOG_ERROR("Nullptr as entity!");
		return;
	}

	rp3d::CollisionBody* entityBody = getUnusedBody();

	rp3d::Vector3 pos = toReactVec(entity->getTransform()->getPosition());
	rp3d::Quaternion rotation = rp3d::Quaternion::identity(); //<--------CHANGE THIS TO ENTITY ROTATION WHEN WE HAVE QUATERNIONS
	rp3d::Transform transform(pos, rotation);

	/*
	SET SHAPE TO COLLISIONBODY. IF SHAPE IS ARROW/PLAYER SAVE THE POINTER FOR COLLISION CHECK
	*/
	rp3d::ProxyShape* proxy;
	switch (shape)
	{
	case SHAPE::ARROW:
	{
		this->player = entityBody;
		// SET SHAPE!
		rp3d::Transform transform = rp3d::Transform::identity();
		proxy = entityBody->addCollisionShape(this->shapes[(size_t)shape], transform);
		break;
	}
	case SHAPE::BOX:
	{
		// Set shapes
		rp3d::Transform transform = rp3d::Transform::identity();
		proxy = entityBody->addCollisionShape(this->shapes[(size_t)shape], transform);
		break;
	}
	case SHAPE::DRONE:
	{
		// SET SHAPE!
		rp3d::Transform transform = rp3d::Transform::identity();
		proxy = entityBody->addCollisionShape(this->shapes[(size_t)shape], transform);
		break;
	}
	default:
		LOG_ERROR("Unknown shape");
	}

	entityBody->setTransform(transform);

	entity->setCollisionBody(entityBody);
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
	rp3d::BoxShape* drone = new rp3d::BoxShape({ 1.0, 1.0, 1.0 });
	this->shapes.push_back(drone);

	// BOX = 1
	rp3d::BoxShape * box = new rp3d::BoxShape({ 1.0, 1.0, 1.0 });
	this->shapes.push_back(box);

	// ARROW = 2 ---- CHANGE TO A MESH WHEN DOWN
	rp3d::SphereShape* arrow = new rp3d::SphereShape(1.0);
	this->shapes.push_back(arrow);
}
