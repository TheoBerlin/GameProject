#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Collision/CollisionHandler.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/Phases/AimPhase.h>
#include <Game/GameLogic/Phases/ReplayPhase.h>
#include <Engine/GUI/GUI.h>
#include <Engine/GUI/Panel.h>

GuidingPhase::GuidingPhase(AimPhase* aimPhase)
	:Phase((Phase*)aimPhase),
	flightTimer(0.0f),
	flightTime(0.0f),
	hasCollided(false)
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

	// Create target panel
	initTargetPanel();

	EventBus::get().subscribe(this, &GuidingPhase::playerCollisionCallback);
    EventBus::get().subscribe(this, &GuidingPhase::handleKeyInput);
}

void GuidingPhase::update(const float& dt)
{
    flightTimer += dt;

	if (this->playerArrow->getTransform()->getPosition().y > level.levelStructure->getWallHeight() && !this->hasCollided)
		beginReplayTransition();
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
	this->hasCollided = true;

    EventBus::get().unsubscribe(this, &GuidingPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &GuidingPhase::playerCollisionCallback);

    level.replaySystem->stopRecording();

	level.scoreManager->stop();

	level.gui->removePanel(this->targetPnl);

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

    this->transitionStraightPath(currentCamSettings, newCamSettings);

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
			updateTargetPanel();
			break;
		}
		case CATEGORY::DRONE_EYE:
		{
			level.scoreManager->scoreBonus();
			updateTargetPanel();
			break;
		}
	}
}

void GuidingPhase::initTargetPanel()
{
	// Create container panel
	this->targetPnl = new Panel();
	this->targetPnl->setOption(GUI::FLOAT_LEFT);
	this->targetPnl->setOption(GUI::FLOAT_UP);
	this->targetPnl->setSize({ 400, 50 });
	this->targetPnl->setColor({ 0.1f, 0.1f, 0.1f, 0.0f });
	level.gui->addPanel(this->targetPnl);

	// Create icon panel
	Panel* icon = new Panel();
	icon->setOption(GUI::FLOAT_LEFT);
	icon->setOption(GUI::FLOAT_UP, 5);
	Texture* tex = TextureManager::loadTexture("./Game/Assets/droneIcon.png");
	icon->setBackgroundTexture(tex);
	icon->setOption(GUI::SCALE_TEXTURE_TO_HEIGHT, 40);
	icon->setColor({ 1.0f, 1.0f, 1.0f, .97f });
	this->targetPnl->addChild(icon);

	// Create text panel
	float ratio = (float)tex->getWidth() / (float)tex->getHeight();
	Panel* textPnl = new Panel();
	textPnl->setOption(GUI::TEXT_FLOAT_LEFT, 20);
	textPnl->setOption(GUI::TEXT_CENTER_Y);
	textPnl->setOption(GUI::SCALE_TO_TEXT_X, 20);
	textPnl->setOption(GUI::SCALE_TO_TEXT_Y);
	textPnl->setPosition({ (unsigned)40*ratio, 0 });
	textPnl->setColor({ 0.1f, 0.1f, 0.1f, 0.0f });
	textPnl->addText("0/" + std::to_string(level.targetManager->getTargetCount()), "aldo", { 1.f, 1.f, 1.f, .97f });
	this->targetPnl->addChild(textPnl);
}

void GuidingPhase::updateTargetPanel()
{
	this->targetPnl->getChildren()[1]->updateText(0, std::to_string(level.scoreManager->getTargetsHit()) + "/" + std::to_string(level.targetManager->getTargetCount()));
}
