#pragma once

#include <Engine/Components/Camera.h>
#include <Game/GameLogic/Phases/Phase.h>
#include <Game/Components/ArrowGuider.h>

class OverviewPhase;
class ReplayPhase;

class AimPhase : public Phase
{
public:
    AimPhase(OverviewPhase* overviewPhase);
    AimPhase(ReplayPhase* replayPhase);
	~AimPhase();

	void update(const float& dt);

    Entity* getPlayerArrow() const;
    ArrowGuider* getArrowGuider() const;
    Camera* getArrowCam() const;

private:
    void commonSetup();

    void handleMouseClick(MouseClickEvent* event);
    void handleKeyInput(KeyEvent* event);

    void transitionToOverview(CameraTransitionEvent* event);

    Entity* playerArrow;
    ArrowGuider* arrowGuider;
    Camera* arrowCam;
};
