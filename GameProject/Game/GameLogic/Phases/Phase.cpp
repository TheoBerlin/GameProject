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
	ParticleManager::get().update(dt);

	// Update entities.
	level.entityManager->update(dt);

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	/*
		Update shaders
	*/
	renderer.updateShaders(dt);
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

    // Get the starting and destinnation camera positions by accounting for offset
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

glm::vec3 Phase::calculatePosition(const CameraSetting& camSettings)
{
    glm::vec3 horizontalRight = glm::normalize(glm::cross(camSettings.direction, GLOBAL_UP_VECTOR));

    glm::vec3 realOffset = camSettings.direction * camSettings.offset.z +
    horizontalRight * camSettings.offset.x + GLOBAL_UP_VECTOR * camSettings.offset.y;

    return camSettings.position + realOffset;
}
