#include "CameraTransition.h"

#include <Engine/Components/Camera.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Events/Events.h>
#include <Utils/Logger.h>
#include <glm/gtx/quaternion.hpp>

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
    std::queue<KeyPoint> newPath;

    newPath.push(KeyPoint(newPos, transitionLength));

    this->setPath(newPath, newForward, newFOV);
}

void CameraTransition::setPath(const std::queue<KeyPoint>& path, const glm::vec3& newForward, float newFOV)
{
    if (path.empty()) {
        LOG_WARNING("Transition path is empty!");

        this->isTransitioning = false;
        return;
    }

    this->isTransitioning = true;
    this->transitionTime = 0.0f;
    this->beginT = 0.0f;
    this->path = path;

    // Calculate final rotation quaternion
    Transform* transform = host->getTransform();

    this->defaultForward = transform->getDefaultForward();

    this->beginQuat = transform->getRotationQuat();

    this->beginPos = transform->getPosition();

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

    Transform* transform = host->getTransform();

    transitionTime += dt;

    while (transitionTime > path.front().t) {
        // The next point in the path has been reached
        path.pop();

        if (path.size() > 0) {
            this->beginPos = transform->getPosition();
            this->beginT = transitionTime;
        }

        else {
            // Transition is finished
            isTransitioning = false;

            // Publish camera transition event
            EventBus::get().publish(&CameraTransitionEvent(host));

            return;
        }
    }

    // Get the slerp factor T in [0,1]
    float T = transitionTime / path.back().t;

    // Calculate current rotation quat
    glm::quat currentRotationQuat = glm::slerp(beginQuat, endQuat, T);

    // Calculate current position
    // The position interpolation needs a different T than the rotation
    T = (transitionTime - beginT) / (path.front().t - beginT);

    glm::vec3 currentPosition = glm::mix(beginPos, path.front().Position, T);

    // Set new rotation and position
    transform->setForward(currentRotationQuat * host->getTransform()->getDefaultForward());
	transform->setPosition(currentPosition);

    // Lerp FOV if the entity has a camera
    if (!entityCam) {
        return;
    }

    float currentFOV = glm::mix(beginFOV, endFOV, T);

    entityCam->setFOV(currentFOV);
}
