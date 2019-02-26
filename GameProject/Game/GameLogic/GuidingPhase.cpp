#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Collision/CollisionHandler.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Game/GameLogic/ReplayPhase.h>

#include <Utils/Logger.h>

GuidingPhase::GuidingPhase(AimPhase* aimPhase)
    :Phase((Phase*)aimPhase)
{
	this->playerArrow = aimPhase->getPlayerArrow();

    arrowCam = aimPhase->getArrowCam();

    // Start guiding the arrow
    arrowGuider = aimPhase->getArrowGuider();
    arrowGuider->startGuiding();

    level.targetManager->resetTargets();

	/*
	Do stuff when collision happens
	*/

	level.collisionHandler->addCollisionToEntity(this->playerArrow, SHAPE::ARROW);

	// Begin recording collisions
	level.replaySystem->startRecording();

	EventBus::get().subscribe(this, &GuidingPhase::playerCollisionCallback);
    EventBus::get().subscribe(this, &GuidingPhase::handleKeyInput);
}

Entity* GuidingPhase::getPlayerArrow() const
{
    return playerArrow;
}

ArrowGuider* GuidingPhase::getArrowGuider() const
{
    return arrowGuider;
}

void GuidingPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_3) {
        beginReplayTransition();
    }
}

void GuidingPhase::beginReplayTransition()
{
    EventBus::get().unsubscribe(this, &GuidingPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &GuidingPhase::playerCollisionCallback);

    arrowGuider->stopGuiding();
    level.replaySystem->stopRecording();

    // Begin camera transition to the replay freecam
    CameraSetting currentCamSettings;

    Transform* arrowTransform = playerArrow->getTransform();

    currentCamSettings.position = arrowTransform->getPosition();
    currentCamSettings.direction = arrowTransform->getForward();
    currentCamSettings.offset = arrowCam->getOffset();
    currentCamSettings.FOV = arrowCam->getFOV();

    CameraSetting newCamSettings = level.player.replayCamera;

    this->setupTransition(currentCamSettings, newCamSettings);

    EventBus::get().subscribe(this, &GuidingPhase::finishReplayTransition);
}

void GuidingPhase::finishReplayTransition(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &GuidingPhase::finishReplayTransition);

	level.collisionHandler->removeCollisionBody(this->playerArrow);

    Phase* guidingPhase = new ReplayPhase(this);
    changePhase(guidingPhase);
}

void GuidingPhase::playerCollisionCallback(PlayerCollisionEvent * ev)
{
	// Check if the arrow hit static geometry
    unsigned int category = ev->shape2->getCollisionCategoryBits();

    if (category == CATEGORY::STATIC) {
        beginReplayTransition();
    }
}
