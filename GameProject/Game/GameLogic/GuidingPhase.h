#pragma once

#include <Game/GameLogic/Phase.h>
#include <Game/Components/ArrowGuider.h>

// Phases GuidingPhase can transition from
class AimPhase;

class GuidingPhase : public Phase
{
public:
    GuidingPhase(AimPhase* other);

    Entity* getPlayerArrow() const;
    ArrowGuider* getArrowGuider() const;

private:
    void handleKeyInput(KeyEvent* event);

    void beginReplayTransition();
    void finishReplayTransition(CameraTransitionEvent* event);

	void playerCollisionCallback(PlayerCollisionEvent * ev);

    Entity* playerArrow;
    ArrowGuider* arrowGuider;
    Camera* arrowCam;
};
