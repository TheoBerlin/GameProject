#pragma once

#include <Game/GameLogic/GameEvents.h>
#include <Game/Components/PathTreader.h>
#include <Utils/Timer.h>
#include <vector>

struct Level;

struct CollisionReplay {
    CollisionReplay(PlayerCollisionEvent event, float time) : event(event), time(time) {};

    PlayerCollisionEvent event;
    float time;
};

class ReplaySystem
{
public:
    ReplaySystem();
    ~ReplaySystem();

    // Used when replaying
    void update(const float& dt);

    // Start recording a playthrough
    void startRecording();
    void stopRecording();
    void deleteReplay();

    // Start replaying a playthrough
    void startReplaying();
    void stopReplaying();

    // Rewind or fast forward a level to its state at a given time
    void setReplayTime(Level& level, PathTreader* replayArrow, Entity* playerEntity, const float time);

    void setCollisionReplays(const std::vector<CollisionReplay>& collisions);

	// Get collision replay vector
	std::vector<CollisionReplay>& getCollisionReplays();

private:
    void handlePlayerCollision(PlayerCollisionEvent* event);

    bool isRecording, isReplaying;
    Timer recordingTimer;

    float replayTime;

    std::vector<CollisionReplay> collisions;
    // Points at the collision about to be republished
    unsigned int collisionIndex;
};
