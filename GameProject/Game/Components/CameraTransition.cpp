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
    this->isTransitioning = true;
    this->transitionTime = 0.0f;
    this->transitionLength = transitionLength;
    this->endPos = newPos;

    Transform* transform = host->getTransform();

    this->beginPos = transform->getPosition();
    this->defaultForward = transform->getDefaultForward();

    // Apply yaw and pitch to the current rotation quat to calculate the end quat
    this->beginQuat = transform->getRotationQuat();

	this->endQuat = glm::rotation(transform->getDefaultForward(), newForward);

    // Attempt to get entity camera to read and write FOV
    entityCam = dynamic_cast<Camera*>(host->getComponent("Camera"));

    if (!entityCam) {
        LOG_WARNING("Failed to find camera component, will not edit FOV");
        return;
    }

    beginFOV = entityCam->getFOV();
    endFOV = newFOV;
}

void CameraTransition::update(const float& dt)
{
    if (!isTransitioning) {
        return;
    }

    transitionTime += dt;

    if (transitionTime > transitionLength) {
        isTransitioning = false;

        // Publish camera transition event
        EventBus::get().publish(&CameraTransitionEvent(host));

		return;
    }

    // Get the slerp factor T in [0,1]
    float T = transitionTime / transitionLength;

    // Calculate current rotation quat
    glm::quat currentRotationQuat = glm::slerp(beginQuat, endQuat, T);

    // Calculate current position
    glm::vec3 currentPosition = glm::mix(beginPos, endPos, T);

    // Set new rotation and position
    Transform* transform = host->getTransform();

    transform->setForward(currentRotationQuat * host->getTransform()->getDefaultForward());
	transform->setPosition(currentPosition);

    // Lerp FOV if the entity has a camera
    if (!entityCam) {
        return;
    }

    float currentFOV = glm::mix(beginFOV, endFOV, T);

    entityCam->setFOV(currentFOV);
}
