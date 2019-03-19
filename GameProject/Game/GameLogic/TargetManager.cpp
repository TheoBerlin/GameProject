#include "TargetManager.h"

#include <Engine/Entity/Entity.h>
#include <Game/Components/Hover.h>
#include <Game/Components/Explosion.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/RollNullifier.h>
#include <Game/Components/DeathAnimation.h>
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
	staticTarget.deathAnimation = new DeathAnimation(host);

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

	movingTarget.pathTreader = new PathTreader(host, path, true);
	movingTarget.rollNullifier = new RollNullifier(host);
	movingTarget.explosion = new Explosion(host);
	movingTarget.deathAnimation = new DeathAnimation(host);
	new MovingTargetCollision(host);

	movingTargets.push_back(movingTarget);
}

void TargetManager::addKeyPoint(Entity * host, const KeyPoint point)
{
	for (int i = 0; i < movingTargets.size(); i++) {
		if (movingTargets[i].pathTreader->getHost()->getName() == host->getName()) {
			std::vector<KeyPoint> path = movingTargets[i].pathTreader->getPath();
			path.push_back(point);
			movingTargets[i].pathTreader->setPath(path);
		}
	}
}

void TargetManager::removeTarget(std::string name)
{
	bool found = false;
	for (int i = 0; i < movingTargets.size() && !found; i++) {
		if (movingTargets[i].pathTreader->getHost()->getName() == name) {
			movingTargets[i].pathTreader->getHost()->removeComponent("RollNullifier");
			movingTargets[i].pathTreader->getHost()->removeComponent("PathThreader");
			movingTargets.erase(movingTargets.begin() + i);
			found = true;
		}
	}
	for (int i = 0; i < staticTargets.size() && !found; i++) {
		if (staticTargets[i].hoverAnimation->getHost()->getName() == name) {
			staticTargets[i].hoverAnimation->getHost()->removeComponent("StaticTargetCollision");
			staticTargets[i].hoverAnimation->getHost()->removeComponent("Hover");
			staticTargets.erase(staticTargets.begin() + i);
			found = true;
		}
	}
}

std::vector<MovingTarget> TargetManager::getMovingTargets() const
{
	return movingTargets;
}

std::vector<StaticTarget> TargetManager::getStaticTargets() const
{
	return staticTargets;
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

std::vector<Entity*> TargetManager::getTargetEntities() const
{
	std::vector<Entity*> targetEntities;

	targetEntities.resize(this->movingTargets.size() + this->staticTargets.size());

	// Add static targets
	for (unsigned int targetIndex = 0; targetIndex < staticTargets.size(); targetIndex += 1) {
		targetEntities[targetIndex] = staticTargets[targetIndex].hoverAnimation->getHost();
	}

	// Add moving targets
	for (unsigned int targetIndex = 0; targetIndex < movingTargets.size(); targetIndex += 1) {
		targetEntities[targetIndex + staticTargets.size()] = movingTargets[targetIndex].pathTreader->getHost();
	}

	return targetEntities;
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
		Entity* targetHost = staticTargets.at(i).hoverAnimation->getHost();
		targetHost->unpauseModelTransform();
		staticTargets.at(i).hoverAnimation->reset();
		staticTargets.at(i).explosion->reset();
		staticTargets.at(i).deathAnimation->reset();

		//Reset color on entity
		int attachmentIndex = targetHost->getRenderingGroupIndex();
		if (attachmentIndex != -1)
			targetHost->getModel()->updateInstancingSpecificData(&glm::vec3(0.0, 0.0, 0.0)[0], sizeof(glm::vec3),
				attachmentIndex * sizeof(glm::vec3), 0, 2);
	}
}

void TargetManager::resetMovingAnimations()
{
	unsigned int movingTargetCount = movingTargets.size();

	for (unsigned int i = 0; i != movingTargetCount; i += 1) {
		Entity* targetHost = movingTargets.at(i).pathTreader->getHost();
		targetHost->unpauseModelTransform();
		movingTargets.at(i).pathTreader->startTreading();
		targetHost->getTransform()->resetRoll();
		movingTargets.at(i).explosion->reset();
		movingTargets.at(i).deathAnimation->reset();

		//Reset color on entity
		int attachmentIndex = targetHost->getRenderingGroupIndex();
		if (attachmentIndex != -1)
			targetHost->getModel()->updateInstancingSpecificData(&glm::vec3(0.0, 0.0, 0.0)[0], sizeof(glm::vec3),
				attachmentIndex * sizeof(glm::vec3), 0, 2);
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