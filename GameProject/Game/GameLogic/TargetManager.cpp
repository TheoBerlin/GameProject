#include "TargetManager.h"

#include <Engine/Entity/Entity.h>
#include <Game/Components/Hover.h>
#include <Game/Components/Explosion.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/RollNullifier.h>
#include <Engine/Components/MovingTargetCollision.h>
#include <Engine/Components/StaticTargetCollision.h>
#include <reactphysics3d/reactphysics3d.h>
#include <Engine/Collision/CollisionInfo.h>

TargetManager::TargetManager()
{
}

TargetManager::~TargetManager()
{
}

void TargetManager::addStaticTarget(Entity* host, const glm::vec3& position)
{
    // Set position and forward
    Transform* transform = host->getTransform();

    transform->setPosition(position);
    transform->setForward(glm::vec3(0.0f, 0.0f, 1.0f));

    // Generic setup for all targets
    setupTargetGeneric(host);

    // Add component pointers to vector
    StaticTarget staticTarget;

    staticTarget.hoverAnimation = new Hover(host);
	staticTarget.explosion = new Explosion(host);

	new StaticTargetCollision(host);

    staticTargets.push_back(staticTarget);
}

void TargetManager::addMovingTarget(Entity* host, const std::vector<KeyPoint>& path)
{
    // Set position and forward
    Transform* transform = host->getTransform();

    transform->setPosition(path[0].Position);
    transform->setForward(glm::normalize(path[1].Position - path[0].Position));

    // Generic setup for all targets
	setupTargetGeneric(host);

    // Add component pointers to vector
    MovingTarget movingTarget;

    movingTarget.pathTreader = new PathTreader(host, path);
    movingTarget.rollNullifier = new RollNullifier(host);
	new MovingTargetCollision(host);

    movingTargets.push_back(movingTarget);
}

void TargetManager::pauseMovingTargets()
{
	for (MovingTarget& t : this->movingTargets) {
		Entity* targetHost = t.rollNullifier->getHost();

		Transform* transform = targetHost->getTransform();

		glm::vec3 p1 = t.pathTreader->getKeyPoint(0).Position;
		glm::vec3 p2 = t.pathTreader->getKeyPoint(1).Position;
		
		transform->setForward(glm::normalize(p2 - p1));
		transform->setPosition(p1);
		transform->resetRoll();
		targetHost->pauseModelTransform();
	}
}

void TargetManager::unpauseMovingTargets()
{
	for (MovingTarget& t : this->movingTargets)
		t.rollNullifier->getHost()->unpauseModelTransform();
}

void TargetManager::resetTargets()
{
	// Moving targets
    resetMovingTargets();

	// Static targets
    resetStaticTargets();
}

unsigned TargetManager::getTargetCount()
{
	return this->movingTargets.size() + this->staticTargets.size();
}

void TargetManager::setupTargetGeneric(Entity* host)
{
    host->getTransform()->setScale(0.25f);
}

void TargetManager::resetStaticTargets()
{
	// Reset collisions
	resetStaticCollisions();

	// Reset animations
	resetStaticAnimations();
}

void TargetManager::resetMovingTargets()
{
	// Reset collisions
	resetMovingCollisions();

	// Reset animations
	resetMovingAnimations();
}

void TargetManager::resetStaticAnimations()
{
	unsigned int staticTargetCount = staticTargets.size();

    for (unsigned int i = 0; i != staticTargetCount; i += 1) {
        staticTargets.at(i).hoverAnimation->reset();
		staticTargets.at(i).explosion->reset();
		
		//Reset color on entity
		Entity* host = staticTargets.at(i).hoverAnimation->getHost();
		int attachmentIndex = host->getRenderingGroupIndex();
		if(attachmentIndex != -1)
			host->getModel()->updateInstancingSpecificData(&glm::vec3(0.0, 0.0, 0.0)[0], sizeof(glm::vec3),
				attachmentIndex *sizeof(glm::vec3), 0, 2);
    }
}

void TargetManager::resetMovingAnimations()
{
	unsigned int movingTargetCount = movingTargets.size();

    for (unsigned int i = 0; i != movingTargetCount; i += 1) {
        movingTargets.at(i).pathTreader->startTreading();
    }
}

void TargetManager::resetStaticCollisions()
{
	rp3d::CollisionBody* body;
	Entity* host;
	for (auto target : this->staticTargets)
	{
		host = target.hoverAnimation->getHost();
		body = host->getCollisionBody();

		rp3d::ProxyShape* current = body->getProxyShapesList();

		StaticTargetCollision* comp = dynamic_cast<StaticTargetCollision*>(host->getComponent("StaticTargetCollision"));
		comp->enableCollision();

		// Look for the proxy shape that contains the collision shape in parameter
		while (current != nullptr) {
			current->setCollisionCategoryBits(((CollisionShapeDrawingData*)current->getUserData())->category);

			// Get the next element in the list
			current = current->getNext();
		}
	}
}

void TargetManager::resetMovingCollisions()
{
	rp3d::CollisionBody* body;
	Entity* host;
	for (auto target : this->movingTargets)
	{
		host = target.pathTreader->getHost();
		body = host->getCollisionBody();

		rp3d::ProxyShape* current = body->getProxyShapesList();

		MovingTargetCollision* comp = dynamic_cast<MovingTargetCollision*>(host->getComponent("MovingTargetCollision"));
		comp->enableCollision();

		// Look for the proxy shape that contains the collision shape in parameter
		while (current != nullptr) {
			current->setCollisionCategoryBits(((CollisionShapeDrawingData*)current->getUserData())->category);

			// Get the next element in the list
			current = current->getNext();
		}
	}
}
