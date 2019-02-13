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
}


CollisionHandler::~CollisionHandler()
{
	delete this->world;

	this->bodies.clear();
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
	rp3d::CollisionBody* entityBody = getUnusedBody();

	rp3d::Vector3 pos = toReactVec(entity->getTransform()->getPosition());
	rp3d::Quaternion rotation = rp3d::Quaternion::identity(); //<--------CHANGE THIS TO ENTITY ROTATION WHEN WE HAVE QUATERNIONS
	rp3d::Transform transform(pos, rotation);

	/*
	SET SHAPE TO COLLISIONBODY. IF SHAPE IS ARROW/PLAYER SAVE THE POINTER FOR COLLISION CHECK
	*/
	switch (shape)
	{
	case SHAPE::ARROW:
	{
		this->player = entityBody;
		// SET SHAPE!
		break;
	}
	case SHAPE::BOX:
	{
		// Set shapes
		rp3d::Transform transform = rp3d::Transform::identity();
		rp3d::ProxyShape* boxShape = entityBody->addCollisionShape(&this->shape, transform);
		break;
	}
	case SHAPE::DRONE:
		// SET SHAPE!
		break;
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
	for (size_t i = 0; i < this->bodies.size(); i++)
	{
		if (this->bodies[i] == body)
		{
			this->world->destroyCollisionBody(body);
			this->bodies[i] = this->bodies[this->bodies.size() - 1];
			this->bodies.pop_back();
			this->takenBodies--;
			entity->setCollisionBody(nullptr);
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
