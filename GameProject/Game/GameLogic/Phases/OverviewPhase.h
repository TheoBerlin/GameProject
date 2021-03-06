#pragma once

#include <Engine/Entity/Entity.h>
#include <Game/GameLogic/Phases/Phase.h>
#include <Game/Components/OversightController.h>

class AimPhase;

class OverviewPhase : public Phase
{
public:
    OverviewPhase(AimPhase* aimPhase);
    OverviewPhase(const Level& level, Entity* transitionEntity);
	~OverviewPhase();

    Entity* getOverviewCamera() const;
    Entity* getPlayerArrow() const;

private:
    void commonSetup();

    void handleKeyInput(KeyEvent* event);

    void transitionToAim(CameraTransitionEvent* event);

    Entity* playerArrow;

    Entity* overviewCamera;
    OversightController* overviewControl;
};
