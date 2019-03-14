#include "CameraDrift.h"

#include <Engine/Entity/Entity.h>
#include <Utils/Logger.h>
#include <glm/gtx/vector_angle.hpp>

CameraDrift::CameraDrift(Entity* host, float angleCorrectionFactor, float maxAngle)
    :Component(host, "CameraDrift"),
    angleCorrectionFactor(angleCorrectionFactor),
    maxAngle(maxAngle)
{
    Component* tmp = host->getComponent("Camera");

    if (!tmp) {
        LOG_WARNING("Could not find camera component");
        return;
    }

    this->camera = dynamic_cast<Camera*>(tmp);

    camera->decouple();
}

CameraDrift::~CameraDrift()
{
}

void CameraDrift::update(const float& dt)
{
    if (!camera) {
        return;
    }

    Transform* transform = host->getTransform();

    glm::vec3 entityForward = transform->getForward();
    glm::vec3 camForward = camera->getForward();

    float cosAngle = glm::dot(camForward, entityForward);

    // Do nothing if the angle is small
    if (cosAngle < 1.0f - 0.0001f) {
        glm::vec2 yawPitchDiff;

        // Calculate yaw difference
        glm::vec3 horizEntityForward = glm::normalize(glm::vec3(entityForward.x, 0.0f, entityForward.z));
        glm::vec3 horizCamForward = glm::normalize(glm::vec3(camForward.x, 0.0f, camForward.z));

        yawPitchDiff.x = glm::orientedAngle(horizCamForward, horizEntityForward, GLOBAL_UP_VECTOR);

        // Calculate pitch difference
        float entityPitch = transform->getPitch();

        float camPitch = glm::orientedAngle(horizCamForward, camForward, camera->getRight());

        yawPitchDiff.y = entityPitch - camPitch;

        float angle = glm::length(yawPitchDiff);

        // Expressed in (yaw, pitch)
        glm::vec2 correctionRotation;

        if (angle > maxAngle) {
            // The angle differential is too large, snap the forward to the maximum allowed forward differential
            correctionRotation = glm::max(angleCorrectionFactor * dt, angle - maxAngle) * yawPitchDiff;
        } else {
            // Smoothly rotate the forward vector
            correctionRotation = glm::min(angleCorrectionFactor * dt, angle) * yawPitchDiff;
        }

        glm::quat rotationQuat = {1.0f, 0.0f, 0.0f, 0.0f};

        // Yaw
        rotationQuat = glm::angleAxis(correctionRotation.x, GLOBAL_UP_VECTOR);

        camForward = rotationQuat * camForward;

        // Pitch
        rotationQuat = glm::angleAxis(correctionRotation.y, camera->getRight());

        camForward = rotationQuat * camForward;

        camera->setForward(camForward);
    }

    glm::vec3 camOffset = camera->getOffset();

    float offsetDistance = glm::length(camOffset);

    if (offsetDistance > 0.01f) {
        glm::vec3 entityPos = transform->getPosition();

        glm::vec3 desiredPosition = entityPos + (transform->getRight() * camOffset.x + transform->getUp() * camOffset.y + entityForward * camOffset.z);

        glm::vec3 lookAt = desiredPosition + entityForward * offsetDistance;

        camera->setPosition(lookAt - camForward * offsetDistance);
    } else {
        camera->setPosition(transform->getPosition());
    }
}
