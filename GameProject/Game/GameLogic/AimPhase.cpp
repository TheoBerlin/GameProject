#include "AimPhase.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/GameLogic/GuidingPhase.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/GameLogic/ReplayPhase.h>
#include <GLFW/glfw3.h>

AimPhase::AimPhase(OverviewPhase* overviewPhase)
    :Phase((Phase*)overviewPhase)
{
    // Get player arrow
    playerArrow = overviewPhase->getPlayerArrow();

    // Remove overview camera
    Entity* overviewCamera = overviewPhase->getOverviewCamera();
    level.entityManager->removeTracedEntity(overviewCamera->getName());

    commonSetup();
}

AimPhase::AimPhase(ReplayPhase* replayPhase)
    :Phase((Phase*)replayPhase)
{
    // Remove path visualizers
    PathVisualizer* pathVisualizer = replayPhase->getPathVisualizer();

    if (pathVisualizer) {
        replayPhase->getPathVisualizer()->removeVisualizers();
    }

    // Remove replay arrow
    Entity* replayArrow = replayPhase->getReplayArrow();

    level.entityManager->removeTracedEntity(replayArrow->getName());

    // Remove freecam
    Entity* freeCam = replayPhase->getFreeCam();

    level.entityManager->removeTracedEntity(freeCam->getName());

    /*
		Create arrow entity
	*/
	Model * model = ModelLoader::loadModel("./Game/assets/Arrow.fbx");

    playerArrow = level.entityManager->addTracedEntity("PlayerArrow");

    Transform* playerTransform = playerArrow->getTransform();

    playerTransform->setForward(level.player.arrowCamera.direction);
    playerTransform->resetRoll();
	playerTransform->setPosition(level.player.arrowCamera.position);
	playerTransform->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	playerArrow->setModel(model);

    commonSetup();
}

Entity* AimPhase::getPlayerArrow() const
{
    return playerArrow;
}

ArrowGuider* AimPhase::getArrowGuider() const
{
    return arrowGuider;
}

Camera* AimPhase::getArrowCam() const
{
    return arrowCam;
}

void AimPhase::commonSetup()
{
	/*
		Add camera to arrow entity
	*/
    CameraSetting arrowCamSettings = level.player.arrowCamera;

    glm::vec3 camOffset = arrowCamSettings.offset;
	arrowCam = new Camera(playerArrow, "Camera", camOffset);

    arrowCam->setFOV(arrowCamSettings.FOV);
	arrowCam->init();

	/*
		Add arrowguider to entity
	*/
	arrowGuider = new ArrowGuider(playerArrow, arrowCamSettings.offset, arrowCamSettings.FOV, 3.0f);
	arrowGuider->startAiming();

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(arrowCam);

    EventBus::get().subscribe(this, &AimPhase::handleKeyInput);
    EventBus::get().subscribe(this, &AimPhase::handleMouseClick);
}

void AimPhase::handleMouseClick(MouseClickEvent* event)
{
    // Change phase to guiding phase when the left mouse button has been released
    if (event->action == GLFW_RELEASE && event->button == GLFW_MOUSE_BUTTON_LEFT) {
        EventBus::get().unsubscribe(this, &AimPhase::handleKeyInput);
        EventBus::get().unsubscribe(this, &AimPhase::handleMouseClick);

        Phase* newPhase = new GuidingPhase(this);

        changePhase(newPhase);
    }
}

void AimPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_1) {
        EventBus::get().unsubscribe(this, &AimPhase::handleKeyInput);
        EventBus::get().unsubscribe(this, &AimPhase::handleMouseClick);

        arrowGuider->stopAiming();

        // Begin camera transition to the oversight camera
        CameraSetting currentCamSettings = level.player.arrowCamera;

        Transform* arrowTransform = playerArrow->getTransform();

        currentCamSettings.position = arrowTransform->getPosition();
        currentCamSettings.direction = arrowTransform->getForward();

        CameraSetting newCamSettings = level.player.oversightCamera;

        this->setupTransition(currentCamSettings, newCamSettings);

        EventBus::get().subscribe(this, &AimPhase::transitionToOverview);
    }
}

void AimPhase::transitionToOverview(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &AimPhase::transitionToOverview);

    Phase* overviewPhase = new OverviewPhase(this);
    changePhase(overviewPhase);
}
