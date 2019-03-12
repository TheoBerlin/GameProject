#include "Phase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Entity/Entity.h>
#include <queue>

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

void Phase::update(const float & dt)
{
}

Phase::~Phase()
{
}

void Phase::changePhase(Phase* newPhase)
{
    EventBus::get().publish(&PhaseChangeEvent(newPhase));
}

void Phase::transitionStraightPath(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings)
{
    // Calculate current camera position
    glm::vec3 currentPos = this->calculatePosition(currentCamSettings);

    // Calculate new camera position
    glm::vec3 newPos = this->calculatePosition(newCamSettings);

    float transitionLength = 2.0f;

    transitionEntity->getTransform()->setPosition(currentPos);
    transitionEntity->getTransform()->setForward(currentCamSettings.direction);

    transitionCam->setFOV(currentCamSettings.FOV);
    transitionCam->setOffset(glm::vec3(0.0f, 0.0f, 0.0f));

    // Update view matrix with new transform settings
    transitionCam->update(0.0f);

    transitionComponent->setDestination(newPos, newCamSettings.direction, newCamSettings.FOV, transitionLength);

    Display::get().getRenderer().setActiveCamera(transitionCam);
}

void Phase::transitionAboveWalls(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings)
{
    float wallHeight = level.levelStructure->getWallHeight();
    float heightMarginFactor = 2.0f;
    float transitionLength = 2.5f;

    // Get the current camera position, account for offset
    glm::vec3 currentPos = this->calculatePosition(currentCamSettings);

    // Height to position far-up points at
    float pathHeight  = wallHeight * 1.2f;

    pathHeight = (currentPos.y > wallHeight * heightMarginFactor) ? currentPos.y : wallHeight * heightMarginFactor;

    std::queue<KeyPoint> transitionPath;

    KeyPoint point;

    // P0: directly above camera, only added if camera is below ceiling
    if (currentPos.y < wallHeight * heightMarginFactor) {
        point.Position = {currentPos.x, pathHeight * 1.2f, currentPos.z};
        point.t = transitionLength / 3.0f;

        transitionPath.push(point);
    }

    glm::vec3 destinationPos = this->calculatePosition(newCamSettings);

    // P1: directly above destination, only added if the camera is currently above the ceiling
    if (currentPos.y > wallHeight * heightMarginFactor) {
        point.Position = {destinationPos.x, pathHeight * 1.2f, destinationPos.z};    
        point.t = transitionLength * 2.0f / 3.0f;

        transitionPath.push(point);
    }

    // P2: destination
    point.Position = destinationPos;
    point.t = transitionLength;

    transitionPath.push(point);

    // Set current camera settins to the transition camera
    transitionEntity->getTransform()->setPosition(currentPos);
    transitionEntity->getTransform()->setForward(currentCamSettings.direction);

    transitionCam->setFOV(currentCamSettings.FOV);
    transitionCam->setOffset(glm::vec3(0.0f, 0.0f, 0.0f));

    // Update view matrix with new transform settings
    transitionCam->update(0.0f);

    transitionComponent->setPath(transitionPath, newCamSettings.direction, newCamSettings.FOV);

    Display::get().getRenderer().setActiveCamera(transitionCam);
}

glm::vec3 Phase::calculatePosition(const CameraSetting& camSettings)
{
    glm::vec3 horizontalRight = glm::normalize(glm::cross(camSettings.direction, GLOBAL_UP_VECTOR));

    glm::vec3 realOffset = camSettings.direction * camSettings.offset.z +
    horizontalRight * camSettings.offset.x + GLOBAL_UP_VECTOR * camSettings.offset.y;

    return camSettings.position + realOffset;
}
