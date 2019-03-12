#pragma once

#include <Game/GameLogic/Phases/Phase.h>
#include <Game/Components/ArrowGuider.h>

// Phases GuidingPhase can transition from
class AimPhase;
class TrailEmitter;

class GuidingPhase : public Phase
{
public:
    GuidingPhase(AimPhase* other);
	~GuidingPhase();

    void update(const float& dt);

    Entity* getPlayerArrow() const;
    ArrowGuider* getArrowGuider() const;
    TrailEmitter* getTrailEmitter() const;
    float getFlightTime();

private:
    void handleKeyInput(KeyEvent* event);

    void beginReplayTransition();
    void finishReplayTransition(CameraTransitionEvent* event);

	void playerCollisionCallback(PlayerCollisionEvent * ev);

    Entity* playerArrow;
    ArrowGuider* arrowGuider;
	TrailEmitter* trailEmitter;
    Camera* arrowCam;

    // Updated each update
    float flightTimer;
    // Updated during collisions or when leaving guiding phase
    float flightTime;
};
