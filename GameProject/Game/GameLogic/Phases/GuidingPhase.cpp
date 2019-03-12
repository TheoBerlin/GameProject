#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Collision/CollisionHandler.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/Phases/AimPhase.h>
#include <Game/GameLogic/Phases/ReplayPhase.h>

GuidingPhase::GuidingPhase(AimPhase* aimPhase)
    :Phase((Phase*)aimPhase),
    flightTimer(0.0f),
    flightTime(0.0f)
{
	this->playerArrow = aimPhase->getPlayerArrow();

    arrowCam = aimPhase->getArrowCam();

    // Start guiding the arrow
    arrowGuider = aimPhase->getArrowGuider();
    arrowGuider->startGuiding();

    level.targetManager->resetTargets();
	level.targetManager->pauseMovingTargets();

	/*
	Do stuff when collision happens
	*/
	level.collisionHandler->addCollisionToEntity(this->playerArrow, CATEGORY::ARROW, true);

	// Begin recording collisions
	level.replaySystem->startRecording();

	// Start scoreManager timer
	level.scoreManager->start();

	EventBus::get().subscribe(this, &GuidingPhase::playerCollisionCallback);
    EventBus::get().subscribe(this, &GuidingPhase::handleKeyInput);
}

void GuidingPhase::update(const float& dt)
{
    flightTimer += dt;
}

GuidingPhase::~GuidingPhase()
{
	EventBus::get().unsubscribe(this, &GuidingPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &GuidingPhase::playerCollisionCallback);
	EventBus::get().unsubscribe(this, &GuidingPhase::finishReplayTransition);
}

Entity* GuidingPhase::getPlayerArrow() const
{
    return playerArrow;
}

ArrowGuider* GuidingPhase::getArrowGuider() const
{
    return arrowGuider;
}

float GuidingPhase::getFlightTime()
{
    return flightTime;
}

void GuidingPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_ESCAPE) {
        EventBus::get().publish(&PauseEvent());
    }

    else if (event->key == GLFW_KEY_3) {
        beginReplayTransition();
    }
}

void GuidingPhase::beginReplayTransition()
{
    EventBus::get().unsubscribe(this, &GuidingPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &GuidingPhase::playerCollisionCallback);

    level.replaySystem->stopRecording();

	level.scoreManager->stop();

    // Get flight time
    flightTime = flightTimer;

    arrowGuider->stopGuiding(flightTime);

    // Begin camera transition to the replay freecam
    CameraSetting currentCamSettings;

    Transform* arrowTransform = playerArrow->getTransform();

    currentCamSettings.position = arrowCam->getPosition();
    currentCamSettings.direction = arrowCam->getForward();
    currentCamSettings.offset = {0.0f, 0.0f, 0.0f};
    currentCamSettings.FOV = arrowCam->getFOV();

    CameraSetting newCamSettings = level.player.arrowCamera;

    // Set the destination's forward to point to the second keypoint in the path
    if (arrowGuider->getPath().size() > 1) {
        newCamSettings.direction = glm::normalize(arrowGuider->getPath()[1].Position - arrowGuider->getPath()[0].Position);
    }

    newCamSettings.offset = {0.0f, 0.0f, -1.6f};

    this->setupTransition(currentCamSettings, newCamSettings);

    EventBus::get().subscribe(this, &GuidingPhase::finishReplayTransition);
}

void GuidingPhase::finishReplayTransition(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &GuidingPhase::finishReplayTransition);

	level.collisionHandler->removeCollisionBody(this->playerArrow);
	level.targetManager->unpauseMovingTargets();

    Phase* guidingPhase = new ReplayPhase(this);
    changePhase(guidingPhase);
}

void GuidingPhase::playerCollisionCallback(PlayerCollisionEvent * ev)
{
	// Save keypoint for collision so that the collision is visible during replay
    flightTime = flightTimer;

    arrowGuider->saveKeyPoint(flightTime);
	// Check if the arrow hit static geometry
    unsigned int category = ev->shape2->getCollisionCategoryBits();

	switch (category)
	{
		case CATEGORY::STATIC:
		{
			beginReplayTransition();
			break;
		}
		case CATEGORY::DRONE_BODY:
		{
			level.scoreManager->score();
			break;
		}
		case CATEGORY::DRONE_EYE:
		{
			level.scoreManager->scoreBonus();
			break;
		}
	}
}
