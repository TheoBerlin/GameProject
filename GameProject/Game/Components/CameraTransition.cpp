#include "CameraTransition.h"

#include <Engine/Entity/Entity.h>

CameraTransition::CameraTransition(Entity* host, const glm::vec3& newPos, const glm::vec3& newForward, float transitionLength)
    :Component(host, "CameraTransition")
{
    Transform* transform = host->getTransform();

    this->beginPos = transform->getPosition();
    this->endPos = newPos;

    transitionTime = 0.0f;

    this->transitionLength = transitionLength;

    // Calculate yaw and pitch required to reach new forward
    glm::vec3 currentForward = transform->getForward();

    glm::vec3 tmpVec = glm::normalize(glm::vec3(newForward.x, currentForward.y, newForward.z));

    float totalYaw = std::acosf(glm::dot(tmpVec, currentForward));

    float totalPitch = std::acosf(glm::dot(tmpVec, newForward));

    // Determine yaw and pitch signs
    totalPitch = (currentForward.y > newForward.y) ? -totalPitch : totalPitch;

    glm::vec3 yawRotationVec = glm::cross(currentForward, newForward);

    totalYaw = (yawRotationVec.y > 0.0f) ? totalYaw : -totalYaw;

    // Apply yaw and pitch to the current rotation quat to calculate the end quat
    this->beginQuat = transform->getRotationQuat();

    // Calculate horizontal right
    glm::vec3 right = transform->getRight();
    glm::vec3 horizontalRight = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

    this->endQuat = glm::angleAxis(totalPitch, horizontalRight) * glm::angleAxis(totalYaw, GLOBAL_UP_VECTOR) * beginQuat;
}

CameraTransition::~CameraTransition()
{
}

void CameraTransition::update(const float& dt)
{
    transitionTime += dt;

    if (transitionTime > transitionLength) {
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

    transform->setRotationQuat(currentRotationQuat);
    transform->setPosition(currentPosition);
}
