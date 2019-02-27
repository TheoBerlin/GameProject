#pragma once

#include <Engine/Events/Events.h>
#include <Utils/Timer.h>
#include <vector>

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

private:
    void handlePlayerCollision(PlayerCollisionEvent* event);

    bool isRecording, isReplaying;
    Timer recordingTimer;

    float replayTime;

    std::vector<CollisionReplay> collisions;
    // Points at the collision about to be republished
    unsigned int collisionIndex;
};
