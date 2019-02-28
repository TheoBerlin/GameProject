#include "Phase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Entity/Entity.h>

Phase::Phase(const Level& level, Entity* transitionEntity)
    :level(level),
    transitionEntity(transitionEntity)
{
    this->transitionComponent = dynamic_cast<CameraTransition*>(transitionEntity->getComponent("CameraTransition"));
    this->transitionCam = dynamic_cast<Camera*>(transitionEntity->getComponent("Camera"));
}

Phase::Phase(Phase* other)
    :level(other->level),
    transitionEntity(other->transitionEntity),
    transitionComponent(other->transitionComponent),
    transitionCam(other->transitionCam)
{
}

void Phase::changePhase(Phase* newPhase)
{
    EventBus::get().publish(&PhaseChangeEvent(newPhase));
}

void Phase::setupTransition(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings)
{
    // Calculate current camera position
    glm::vec3 currentRight = glm::normalize(glm::cross(currentCamSettings.direction, GLOBAL_UP_VECTOR));

    glm::vec3 currentOffset = currentCamSettings.direction * currentCamSettings.offset.z +
    currentRight * currentCamSettings.offset.x + GLOBAL_UP_VECTOR * currentCamSettings.offset.y;

    glm::vec3 currentPos = currentCamSettings.position + currentOffset;

    // Calculate new camera position
    glm::vec3 newRight = glm::normalize(glm::cross(newCamSettings.direction, GLOBAL_UP_VECTOR));

    glm::vec3 destinationOffset = newCamSettings.direction * newCamSettings.offset.z +
    newRight * newCamSettings.offset.x + GLOBAL_UP_VECTOR * newCamSettings.offset.y;

    glm::vec3 newPos = newCamSettings.position + destinationOffset;

    float transitionLength = 2.0f;

    transitionEntity->getTransform()->setPosition(currentPos);
    transitionEntity->getTransform()->setForward(currentCamSettings.direction);

    transitionCam->setPosition(currentPos);
    transitionCam->setForward(currentCamSettings.direction);
    transitionCam->setFOV(currentCamSettings.FOV);
    transitionCam->setOffset(glm::vec3(0.0f, 0.0f, 0.0f));

    transitionComponent->setDestination(newPos, newCamSettings.direction, newCamSettings.FOV, transitionLength);

    Display::get().getRenderer().setActiveCamera(transitionCam);
}
