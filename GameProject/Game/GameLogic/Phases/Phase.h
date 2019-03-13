#pragma once

#include <Game/Level/Level.h>
#include <Engine/Events/Events.h>
#include <Game/Components/CameraTransition.h>
#include <Engine/Components/Camera.h>

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

    Level level;

    // Transition tools
    Entity* transitionEntity;
    Camera* transitionCam;
    CameraTransition* transitionComponent;

private:
    // Calculate the position of a camera by accounting for offset
    glm::vec3 calculatePosition(const CameraSetting& camSettings);
};
