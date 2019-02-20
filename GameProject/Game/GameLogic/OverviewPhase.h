#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Events/Events.h>
#include <Game/GameLogic/Phase.h>
#include <Game/Components/OversightController.h>

class AimPhase;

class OverviewPhase : public Phase
{
public:
    OverviewPhase(AimPhase* aimPhase);
    OverviewPhase(const Level& level);

    Entity* getOverviewCamera() const;

private:
    void commonSetup();

    void handleKeyInput(KeyEvent* event);

    void transitionToAim(CameraTransitionEvent* event);

    Entity* overviewCamera;
    OversightController* oversightControl;
};
