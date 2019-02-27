#include "ReplaySystem.h"

#include <Engine/Events/EventBus.h>

ReplaySystem::ReplaySystem()
    :replayTime(0.0f),
    isRecording(false),
    isReplaying(false)
{
}

ReplaySystem::~ReplaySystem()
{
    EventBus::get().unsubscribe(this, &ReplaySystem::handlePlayerCollision);
}

void ReplaySystem::update(const float& dt)
{
    if (!isReplaying) {
        return;
    }

    replayTime += dt;

    // Advance collisionIndex and republish events
    while (collisionIndex < collisions.size() && collisions[collisionIndex].time < replayTime) {
        EventBus::get().publish(&collisions[collisionIndex].event);

        collisionIndex += 1;
    }

    // Stop replaying if all collision events have been republished
    if (collisionIndex == collisions.size()) {
        stopReplaying();
    }
}

void ReplaySystem::startRecording()
{
    EventBus::get().subscribe(this, &ReplaySystem::handlePlayerCollision);

    // Delete old replay
    if (collisions.size() > 0) {
        deleteReplay();
    }

    // Safety measure to avoid having the replay system record its own replays
    if (isReplaying) {
        stopReplaying();
    }

    recordingTimer.start();
    isRecording = true;
}

void ReplaySystem::stopRecording()
{
    EventBus::get().unsubscribe(this, &ReplaySystem::handlePlayerCollision);

    isRecording = false;

    recordingTimer.stop();
}

void ReplaySystem::deleteReplay()
{
    collisions.clear();
}

void ReplaySystem::startReplaying()
{
    // This will enable update to republish collision events
    isReplaying = true;

    collisionIndex = 0;
    replayTime = 0.0f;

    // Safety measure
    if (isRecording) {
        stopRecording();
    }
}

void ReplaySystem::stopReplaying()
{
    isReplaying = false;
}

void ReplaySystem::handlePlayerCollision(PlayerCollisionEvent* event)
{
    recordingTimer.update();
    float timeStamp = recordingTimer.getTime();

    collisions.push_back(CollisionReplay(*event, timeStamp));
}
