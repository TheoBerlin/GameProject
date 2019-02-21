#include "TargetManager.h"

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

    movingTargets.push_back(movingTarget);
}

void TargetManager::resetTargets()
{
    resetMovingTargets();
    resetStaticTargets();
}

void TargetManager::setupTargetGeneric(Entity* host)
{
    host->getTransform()->setScale(0.25f);
}

void TargetManager::resetStaticTargets()
{
}

void TargetManager::resetMovingTargets()
{
	unsigned int movingTargetCount = movingTargets.size();

    for (unsigned int i = 0; i != movingTargetCount; i += 1) {
        movingTargets.at(i).pathTreader->startTreading();
    }
}
