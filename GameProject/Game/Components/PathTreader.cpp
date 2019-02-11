#include "PathTreader.h"

#include <Engine/Entity/Entity.h>

PathTreader::PathTreader(Entity* host)
    :Component(host, "PathTreader")
{
    init();
}

PathTreader::PathTreader(Entity* host, const std::vector<KeyPoint>& path)
    :Component(host, "PathTreader")
{
    setPath(path);

    init();
}

PathTreader::~PathTreader()
{
}

void PathTreader::update(const float& dt)
{
    if (!isTreading) {
        return;
    }

    pathTime += dt;

    // Check if the end has been reached
    if (pathTime > path.at(path.size() - 1).t) {
        stopTreading();

        return;
    }

    linearTread();
}

void PathTreader::setPath(const std::vector<KeyPoint>& path)
{
    this->path = path;
    currentPointIndex = 0;
}

void PathTreader::beginTreading()
{
    pathTime = 0.0f;

    isTreading = true;

    currentPointIndex = 0;
}

void PathTreader::stopTreading()
{
    isTreading = false;
}

void PathTreader::init()
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
    KeyPoint& P0 = path.at(currentPointIndex);
    KeyPoint& P1 = path.at(currentPointIndex + 1);
    KeyPoint& P2 = path.at(currentPointIndex + 2);
    KeyPoint& P3 = path.at(currentPointIndex + 3);

    float t = pathTime - P0.t;

    float t0 = 0.0f;
    float t1 = calculateT(t0, P0.Position, P1.Position);
    float t2 = calculateT(t1, P1.Position, P2.Position);
    float t3 = calculateT(t2, P2.Position, P3.Position);

    glm::vec3 A1 = (t1 - t)/(t1 - t0) * P0.Position + (t - t0)/(t1 - t0) * P1.Position;
    glm::vec3 A2 = (t2 - t)/(t2 - t1) * P0.Position + (t - t1)/(t2 - t1) * P2.Position;
    glm::vec3 A3 = (t3 - t)/(t3 - t2) * P0.Position + (t - t2)/(t3 - t2) * P3.Position;

    glm::vec3 B1 = (t2 - t)/(t2 - t0) * A1 + (t - t0)/(t2 - t0) * A2;
    glm::vec3 B2 = (t3 - t)/(t3 - t1) * A2 + (t - t1)/(t3 - t1) * A3;

    glm::vec3 newPosition = (t2 - t)/(t2 - t1) * B1 + (t - t1)/(t2 - t1) * B2;

    Transform* transform = host->getTransform();

    glm::vec3 newDirection = glm::normalize(newPosition - transform->getPosition());

    transform->setPosition(newPosition);
    transform->setForward(newDirection);
}

float PathTreader::calculateT(const float& t0, const glm::vec3& P0, const glm::vec3& P1)
{
    return t0 + glm::pow(glm::length(P1 - P0), 0.5f);
}
