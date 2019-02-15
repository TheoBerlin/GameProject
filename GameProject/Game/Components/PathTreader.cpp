#include "PathTreader.h"

#include <Engine/Entity/Entity.h>

PathTreader::PathTreader(Entity* host)
    :Component(host, "PathTreader")
{
    isTreading = false;
}

PathTreader::PathTreader(Entity* host, const std::vector<KeyPoint>& path)
    :Component(host, "PathTreader")
{
    setPath(path);

    isTreading = false;
}

PathTreader::~PathTreader()
{
    stopTreading();
}

void PathTreader::update(const float& dt)
{
    if (!isTreading) {
        return;
    }

	pathTime += dt;

    // Check if the end has been reached
    if (pathTime >= path.at(path.size() - 1).t) {
        stopTreading();

        return;
    }

    catmullRomTread();
}

void PathTreader::setPath(const std::vector<KeyPoint>& path)
{
    this->path = path;
    currentPointIndex = 0;
}

void PathTreader::startTreading()
{
    if (path.size() < 2) {
        LOG_WARNING("Path too small to start treading, size: %d", path.size());
        return;
    }

    pathTime = 0.0f;

    isTreading = true;

    currentPointIndex = 0;
}

void PathTreader::stopTreading()
{
    isTreading = false;
}

void PathTreader::linearTread()
{
    // Update point index
    while (pathTime > path.at(currentPointIndex + 1).t) {
        currentPointIndex += 1;
    }

    // Update position and direction
    KeyPoint& P0 = path.at(currentPointIndex);
    KeyPoint& P1 = path.at(currentPointIndex + 1);

    float timeDiff = P0.t - P1.t;
    glm::vec3 newPosition = P0.Position + (pathTime / timeDiff) * P1.Position;

    Transform* transform = host->getTransform();

    glm::vec3 newDirection = glm::normalize(newPosition - transform->getPosition());

    transform->setPosition(newPosition);
    transform->setForward(newDirection);
}

void PathTreader::catmullRomTread()
{
    // Iterate through points to find P1
    while (currentPointIndex + 2 < path.size() && path.at(currentPointIndex + 1).t < pathTime) {
        currentPointIndex += 1;
    }

    KeyPoint P0 = path.at(glm::max<int>(currentPointIndex - 1, 0));
    KeyPoint P1 = path.at(currentPointIndex);
    KeyPoint P2 = path.at(currentPointIndex + 1);
    KeyPoint P3 = path.at(glm::min<int>(currentPointIndex + 2, path.size() - 1));

    // Get t in [0, 1] using pathTime's relative position in [P1.t, P2.t]
    // t is derived from the following equation: pathTime = P1.t + t * (P2.t-P1.t)
    float t = (pathTime - P1.t) / (P2.t - P1.t);

    Transform* transform = host->getTransform();

    glm::vec3 newPos = glm::catmullRom(P0.Position, P1.Position, P2.Position, P3.Position, t);

	float distance = glm::length(newPos - transform->getPosition());

	if (distance > 0.0f) {
		glm::vec3 newDir = (newPos - transform->getPosition())/distance;

        transform->setForward(newDir);
        transform->setPosition(newPos);
	}
}
