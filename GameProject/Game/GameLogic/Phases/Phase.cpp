#include "Phase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Entity/Entity.h>
#include <vector>

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
    if (this->transitionCamDrift) {
        this->transitionEntity->removeComponent(this->transitionCamDrift->getName());

        this->transitionCamDrift = nullptr;

        this->transitionCam->couple();
    }

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

    // Get the starting and destination camera positions by accounting for offset
    glm::vec3 currentPos = this->calculatePosition(currentCamSettings);
    glm::vec3 destinationPos = this->calculatePosition(newCamSettings);

    // Height to position far-up points at
    float pathHeight  = wallHeight * 1.2f;

    pathHeight = (currentPos.y > wallHeight * heightMarginFactor) ? currentPos.y : wallHeight * heightMarginFactor;

    std::vector<KeyPoint> transitionPath;

    // Set keypoints in the path. Begin by only setting the positions.
    KeyPoint point;

    // P0: current position
    point.Position = currentPos;

    transitionPath.push_back(point);

    // P1: directly above the current camera, only added if the current camera is below the ceiling
    if (currentPos.y < wallHeight * heightMarginFactor) {
        point.Position = {currentPos.x, pathHeight * 1.2f, currentPos.z};

        transitionPath.push_back(point);
    }

    // P1/P2: directly above destination, only added if the target camera is below the ceiling
    if (destinationPos.y < wallHeight * heightMarginFactor) {
        point.Position = {destinationPos.x, pathHeight * 1.2f, destinationPos.z};    

        transitionPath.push_back(point);
    }

    // P2/P3: destination
    point.Position = destinationPos;

    transitionPath.push_back(point);

    // Set keypoint timestamps
    float t = 0.0f;

    for (unsigned int i = 0; i < transitionPath.size(); i += 1) {
        transitionPath[i].t = t;

        t += transitionLength / (transitionPath.size() - 1);
    }

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

void Phase::transitionBackwards(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings, const std::vector<KeyPoint>& path)
{
    // Get the starting and destination camera positions by accounting for offset
    glm::vec3 currentPos = this->calculatePosition(currentCamSettings);
    glm::vec3 destinationPos = this->calculatePosition(newCamSettings);

    // Set current camera settins to the transition camera
    transitionEntity->getTransform()->setPosition(currentPos);
    transitionEntity->getTransform()->setForward(currentCamSettings.direction);

    transitionCam->setFOV(currentCamSettings.FOV);
    transitionCam->setOffset(glm::vec3(0.0f, 0.0f, 0.0f));

    // Update view matrix with new transform settings
    transitionCam->update(0.0f);

    // Revert path
    std::vector<KeyPoint> backwardsPath;
    backwardsPath.resize(path.size() + 1);

    backwardsPath[0].t = 0.0f;
    backwardsPath[0].Position = currentPos;

    // How much faster the backwards path will be traversed, expressed in percent
    float speedUpFactor = 2.0f;

    for (unsigned int i = 1; i < path.size(); i += 1) {
        backwardsPath[i].Position = path[path.size() - 1 - i].Position;

        float timestamp = backwardsPath[i-1].t + (path[path.size() - i].t - path[path.size() - 1 - i].t) / (1.0f + speedUpFactor);
        backwardsPath[i].t = timestamp;
    }

    // Set the last point to the destination camera
    backwardsPath.back().t = backwardsPath[backwardsPath.size() - 2].t + 0.3f;
    backwardsPath.back().Position = destinationPos;

    transitionComponent->setBackwardsPath(backwardsPath, newCamSettings.direction, newCamSettings.FOV);

    // Smoothen the forward redirects
    float maxAngle = glm::two_pi<float>();
    float angleCorrectionFactor = 2.0f;

    this->transitionCamDrift = new CameraDrift(this->transitionEntity, angleCorrectionFactor, maxAngle);

    Display::get().getRenderer().setActiveCamera(transitionCam);
}

glm::vec3 Phase::calculatePosition(const CameraSetting& camSettings)
{
    glm::vec3 horizontalRight = glm::normalize(glm::cross(camSettings.direction, GLOBAL_UP_VECTOR));

    glm::vec3 realOffset = camSettings.direction * camSettings.offset.z +
    horizontalRight * camSettings.offset.x + GLOBAL_UP_VECTOR * camSettings.offset.y;

    return camSettings.position + realOffset;
}
