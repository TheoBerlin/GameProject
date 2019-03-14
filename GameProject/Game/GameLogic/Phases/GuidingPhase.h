#pragma once

#include <Game/GameLogic/Phases/Phase.h>
#include <Game/Components/ArrowGuider.h>

// Phases GuidingPhase can transition from
class AimPhase;
class TrailEmitter;

class Panel;

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
	// True if arrow has collided
	bool hasCollided;
	
	// Key event callback
    void handleKeyInput(KeyEvent* event);

	// Handle transition
    void beginReplayTransition();
    void finishReplayTransition(CameraTransitionEvent* event);

	// Callback for playercollision for score and GUI use
	void playerCollisionCallback(PlayerCollisionEvent * ev);

	// Create target panel 
	void initTargetPanel();
	// Update target panel
	void updateTargetPanel();

    Entity* playerArrow;
    ArrowGuider* arrowGuider;
	TrailEmitter* trailEmitter;
    Camera* arrowCam;

	Panel* targetPnl;

    // Updated each update
    float flightTimer;
    // Updated during collisions or when leaving guiding phase
    float flightTime;
};
