#pragma once

#include <Game/GameLogic/Phase.h>
#include <Game/Components/ArrowGuider.h>

// Phases GuidingPhase can transition from
class AimPhase;

class GuidingPhase : public Phase
{
public:
    GuidingPhase(AimPhase* other);
	~GuidingPhase();

    Entity* getPlayerArrow() const;
    ArrowGuider* getArrowGuider() const;

private:
    void handleKeyInput(KeyEvent* event);

	void playerCollisionCallback(PlayerCollisionEvent * ev);
    void transitionToReplay(CameraTransitionEvent* event);

    Entity* playerArrow;
    ArrowGuider* arrowGuider;
    Camera* arrowCam;
};
