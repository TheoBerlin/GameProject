#include "ReplaySystem.h"

#include <Engine/Entity/Entity.h>
#include <Engine/Events/EventBus.h>
#include <Game/Components/Explosion.h>
#include <Game/Level/Level.h>
#include <Engine/Collision/CollisionInfo.h>

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
		//Adds explosion on collision
		Component * explosionComponent = collisions[collisionIndex].event.entity2->getComponent("Explosion");

		if (explosionComponent) {
			float elapsedTime = replayTime - collisions[collisionIndex].time;

			dynamic_cast<Explosion*>(explosionComponent)->explode(2.0, elapsedTime, collisions[collisionIndex].event.shape2->getCollisionCategoryBits() == CATEGORY::DRONE_EYE);
		}

		collisions[collisionIndex].event.phase = PlayerCollisionEvent::REPLAY_PHASE;
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

void ReplaySystem::setReplayTime(Level& level, PathTreader* replayArrow, Entity* playerEntity, const float time)
{
    /*
        Rewinding the level is done in two steps:
        1. Reset the level to its original state
        2. Fast forward the level to its state at the given time
    */

    // Reset targets
    level.targetManager->resetTargets();

    // Reset replay arrow
    replayArrow->startTreading();

    /*
        The path treader sets the forward to (newPos - oldPos),
        when stepping in time, this will result in weird forward directions.
        Get the expected forward
    */
    replayArrow->update(time - 0.001f);
    replayArrow->update(0.001f);

    Transform* arrowTransform = replayArrow->getHost()->getTransform();

    glm::vec3 arrowForward = arrowTransform->getForward();

    // Reset the arrow again
    replayArrow->startTreading();

    // Reset collision replays
    this->startReplaying();

    // Step forward to each collision and update entities with the delta time between each collision
    float timeStep = 0.0f;

    // Fast forward level, update every entity except the player entity
    std::vector<Entity*> entities = level.entityManager->getAll();

	ParticleManager& particleManager = ParticleManager::get();
	SoundManager& soundManager = SoundManager::get();

	unsigned int collisionsToReplay = 0;

	// Calculate the amount collisions to replay
	while (collisionsToReplay < collisions.size() && collisions[collisionsToReplay].time < time) {
		collisionsToReplay += 1;
	};

    while (this->collisionIndex < collisionsToReplay) {
		// Fast forward sounds, do not include the first timestep (0 -> collisions[0].time)
		soundManager.offsetEffects(timeStep);
		particleManager.update(timeStep);

        timeStep = this->collisions[collisionIndex].time - replayTime + 0.001f;

        for (auto& entity : entities) {
            if (entity != playerEntity) {
                entity->update(timeStep);
            }
        }

        this->update(timeStep);
    }

    // All collisions have been replayed, time-step one last time
	timeStep = time - replayTime;

	if (timeStep > 0.0f) {
		particleManager.update(timeStep);
		soundManager.offsetEffects(timeStep);

		for (auto& entity : entities) {
			if (entity != playerEntity) {
				entity->update(time - replayTime);
			}
		}
	}

	replayTime = time;

    // Set the arrow's expected forward
    arrowTransform->setForward(arrowForward);
}

void ReplaySystem::setCollisionReplays(const std::vector<CollisionReplay>& collisions)
{
	this->collisions = collisions;
}

std::vector<CollisionReplay>& ReplaySystem::getCollisionReplays()
{
	return this->collisions;
}

void ReplaySystem::handlePlayerCollision(PlayerCollisionEvent* event)
{
	recordingTimer.update();
	float timeStamp = recordingTimer.getTime();

	collisions.push_back(CollisionReplay(*event, timeStamp));
}
