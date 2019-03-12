#include "CameraTransition.h"

#include <Engine/Components/Camera.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Events/Events.h>
#include <Utils/Logger.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/spline.hpp>

CameraTransition::CameraTransition(Entity* host)
    :Component(host, "CameraTransition"),
    isTransitioning(false)
{
}

CameraTransition::CameraTransition(Entity* host, const glm::vec3& newPos, const glm::vec3& newForward, float newFOV, float transitionLength)
    :Component(host, "CameraTransition")
{
    setDestination(newPos, newForward, newFOV, transitionLength);
}

CameraTransition::~CameraTransition()
{
}

void CameraTransition::setDestination(const glm::vec3& newPos, const glm::vec3& newForward, float newFOV, float transitionLength)
{
    std::vector<KeyPoint> newPath;

    newPath.push_back(KeyPoint(host->getTransform()->getPosition(), 0.0f));

    newPath.push_back(KeyPoint(newPos, transitionLength));

    this->setPath(newPath, newForward, newFOV);
}

void CameraTransition::setPath(const std::vector<KeyPoint>& path, const glm::vec3& newForward, float newFOV)
{
    if (path.size() < 2) {
        LOG_WARNING("Path too small to start transitioning, size: %d", path.size());

        this->isTransitioning = false;
        return;
    }

    this->isTransitioning = true;
    this->transitionTime = 0.0f;
    this->beginT = 0.0f;
    this->path = path;
    this->pathIndex = 0;

    // Calculate final rotation quaternion
    Transform* transform = host->getTransform();

    this->defaultForward = transform->getDefaultForward();

    this->beginQuat = transform->getRotationQuat();

    // Quaternion for achieving the forward direction for the next point in the path
	this->endQuat = glm::rotation(transform->getDefaultForward(), newForward);

    // Attempt to get entity camera to read and write FOV
    entityCam = dynamic_cast<Camera*>(host->getComponent("Camera"));

    if (!entityCam) {
        LOG_WARNING("Failed to find camera component, will not edit FOV");
        return;
    }

    // Starting FOV and the final FOV for the transition
    this->beginFOV = entityCam->getFOV();
    this->endFOV = newFOV;
}

void CameraTransition::update(const float& dt)
{
    if (!isTransitioning) {
        return;
    }

    transitionTime += dt;

    if (transitionTime > path.back().t) {
        // Transition is finished
        isTransitioning = false;

        // Publish camera transition event
        EventBus::get().publish(&CameraTransitionEvent(host));

        return;
    }

    this->catmullRomMove();

    // Interpolate FOV if the entity has a camera
    if (!entityCam) {
        return;
    }

    // Get the slerp factor T in [0,1]
    float T = transitionTime / path.back().t;

    // Calculate current rotation quat
    glm::quat currentRotationQuat = glm::slerp(beginQuat, endQuat, T);

    host->getTransform()->setForward(currentRotationQuat * host->getTransform()->getDefaultForward());

    float currentFOV = glm::mix(beginFOV, endFOV, T);

    entityCam->setFOV(currentFOV);
}

void CameraTransition::catmullRomMove()
{
    KeyPoint P0, P1, P2, P3;

    // Iterate through points to find P1
    while (pathIndex + 2 < path.size() && path[pathIndex + 1].t < transitionTime) {
        pathIndex += 1;
    }

    P0 = path[glm::max<int>(pathIndex - 1, 0)];
    P1 = path[pathIndex];
    P2 = path[pathIndex + 1];
    P3 = path[glm::min<int>(pathIndex + 2, path.size() - 1)];

    // Calculate current position
    // The position interpolation needs a different T than the rotation
    float T = (transitionTime - P1.t) / (P2.t - P1.t);

    // Set new rotation and position
	host->getTransform()->setPosition(glm::catmullRom(P0.Position, P1.Position, P2.Position, P3.Position, T));
}
