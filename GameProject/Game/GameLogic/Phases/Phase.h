#pragma once

#include <Engine/Components/Camera.h>
#include <Engine/Events/Events.h>
#include <Game/Level/Level.h>
#include <Game/Components/CameraDrift.h>
#include <Game/Components/CameraTransition.h>

class Entity;

class Phase
{
public:
    Phase(const Level& level, Entity* transitionEntity);
    Phase(Phase* other);
	virtual ~Phase();

	virtual void update(const float& dt);

protected:
    void changePhase(Phase* newPhase);

    // Begins camera transition in a straight path
    void transitionStraightPath(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings);
    // Transitions above walls
    void transitionAboveWalls(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings);
    // Transition backwards along a path
    void transitionBackwards(const CameraSetting& currentCamSettings, const CameraSetting& newCamSettings, const std::vector<KeyPoint>& path);

    Level level;

    // Transition tools
    Entity* transitionEntity;
    Camera* transitionCam;
    CameraTransition* transitionComponent;
    CameraDrift* transitionCamDrift;

private:
    // Calculate the position of a camera by accounting for offset
    glm::vec3 calculatePosition(const CameraSetting& camSettings);
};
