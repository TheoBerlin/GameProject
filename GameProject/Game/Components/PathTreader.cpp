#include "PathTreader.h"

#include <Engine/Entity/Entity.h>
#include <Utils/Logger.h>
#include <Utils/Utils.h>
#include <glm/gtx/spline.hpp>

PathTreader::PathTreader(Entity* host)
    :Component(host, "PathTreader")
{
    isTreading = false;
}

PathTreader::PathTreader(Entity* host, const std::vector<KeyPoint>& path, bool loop)
    :Component(host, "PathTreader")
{
    setPath(path, loop);

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
    if (!isLooping && pathTime >= path.back().t) {
        stopTreading();

        return;
    }

    catmullRomTread();
}

void PathTreader::setPath(const std::vector<KeyPoint>& path, bool loop)
{
    this->path = path;
    currentPointIndex = 0;

	this->isLooping = loop;
}

std::vector<KeyPoint>& PathTreader::getPath()
{
    return this->path;
}

KeyPoint & PathTreader::getKeyPoint(unsigned int index)
{
	return path.at(index);
}

const std::vector<KeyPoint>& PathTreader::getPath()
{
	return path;
}

void PathTreader::startTreading()
{
    if (path.size() < 2) {
        LOG_WARNING("Path too small to start treading, size: %d", path.size());
        return;
    }

    isTreading = true;

	pathTime = 0.0f;
	currentPointIndex = 0;
}

void PathTreader::stopTreading()
{
    isTreading = false;
}

unsigned int PathTreader::getCurrentPointIndex() const
{
    return this->currentPointIndex;
}

void PathTreader::catmullRomTread()
{
    KeyPoint P0, P1, P2, P3;

    // Iterate through points to find P1
    while (currentPointIndex + 2 < path.size() && path.at(currentPointIndex + 1).t < pathTime) {
        currentPointIndex += 1;
    }

    if (!isLooping) {
        P0 = path[glm::max<int>(currentPointIndex - 1, 0)];
        P1 = path[currentPointIndex];
        P2 = path[currentPointIndex + 1];
        P3 = path[glm::min<int>(currentPointIndex + 2, path.size() - 1)];
    }

    else {
        while (pathTime >= path.back().t) {
            // Go back to the beginning of the loop if the treader is at the last point
            pathTime -= path.back().t;

            currentPointIndex = 0;

            // Iterate through points to find P1
            while (currentPointIndex + 2 < path.size() && path.at(currentPointIndex + 1).t < pathTime) {
                currentPointIndex += 1;
            }
        }

        // Calculate path indices
        // If the treader is at the first point, set index 0 to the second last point
        // (not the last point, as the last point is on top of the first point)
        P0 = path[currentPointIndex >= 1 ? currentPointIndex - 1 : path.size() - 2];
        P1 = path[currentPointIndex];

        // Avoid out of range
        P2 = path[(currentPointIndex + 1) % path.size()];

        // If the treader is moving towards the last point, set index 3 to 1
        P3 = path[currentPointIndex == path.size() - 2 ? 1 : currentPointIndex + 2];
    }

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
