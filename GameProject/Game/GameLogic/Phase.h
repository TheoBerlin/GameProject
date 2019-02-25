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

protected:
    void changePhase(Phase* newPhase);

    Level level;

    // Transition tools
    Entity* transitionEntity;
    Camera* transitionCam;
    CameraTransition* transitionComponent;
};
