#include "AimPhase.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/TrailEmitter.h>
#include <Game/Components/CameraDrift.h>
#include <Game/GameLogic/Phases/GuidingPhase.h>
#include <Game/GameLogic/Phases/OverviewPhase.h>
#include <Game/GameLogic/Phases/ReplayPhase.h>
#include <Engine/Components/PlayerCollision.h>
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

    /*
		Create arrow entity
	*/
	Model * model = ModelLoader::loadModel("./Game/assets/Arrow.fbx");

    playerArrow = level.entityManager->addTracedEntity("PlayerArrow");

    Transform* playerTransform = playerArrow->getTransform();

    playerTransform->setForward(level.player.arrowCamera.direction);
    playerTransform->resetRoll();
	playerTransform->setPosition(level.player.arrowCamera.position);

	playerArrow->setModel(model);

	new PlayerCollision(playerArrow);

    commonSetup();
}

AimPhase::~AimPhase()
{
	EventBus::get().unsubscribe(this, &AimPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &AimPhase::transitionToOverview);
	EventBus::get().unsubscribe(this, &AimPhase::handleMouseClick);
}

void AimPhase::update(const float & dt)
{

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
    CameraSetting arrowCamSettings = level.player.arrowCamera;

	/*
		Add arrowguider to entity
	*/
	arrowGuider = new ArrowGuider(playerArrow, arrowCamSettings.offset, arrowCamSettings.FOV, 3.0f);
	new TrailEmitter(playerArrow);

	/*
		Add camera to arrow entity
	*/
	glm::vec3 camOffset = arrowCamSettings.offset;
	arrowCam = new Camera(playerArrow, "Camera", camOffset);

	arrowCam->setFOV(arrowCamSettings.FOV);
	arrowCam->init();

    // Smoothen the forward redirects
    float maxAngle = glm::quarter_pi<float>() / 3.5f;
    float angleCorrectionFactor = 4.5f;

    new CameraDrift(playerArrow, angleCorrectionFactor, maxAngle);

	Display::get().getRenderer().setActiveCamera(arrowCam);

	arrowGuider->startAiming();

	// Reset targets
	level.targetManager->resetTargets();

    EventBus::get().subscribe(this, &AimPhase::handleKeyInput);
    EventBus::get().subscribe(this, &AimPhase::handleMouseClick);

    // Lock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

    if (event->key == GLFW_KEY_ESCAPE) {
        EventBus::get().publish(&PauseEvent());
    }

    else if (event->key == GLFW_KEY_1) {
        EventBus::get().unsubscribe(this, &AimPhase::handleKeyInput);
        EventBus::get().unsubscribe(this, &AimPhase::handleMouseClick);

        arrowGuider->stopAiming();

        // Begin camera transition to the oversight camera
        CameraSetting currentCamSettings = level.player.arrowCamera;

        Transform* arrowTransform = playerArrow->getTransform();

        currentCamSettings.position = arrowTransform->getPosition();
        currentCamSettings.direction = arrowTransform->getForward();

        CameraSetting newCamSettings = level.player.oversightCamera;

        this->transitionAboveWalls(currentCamSettings, newCamSettings);

        EventBus::get().subscribe(this, &AimPhase::transitionToOverview);
    }
}

void AimPhase::transitionToOverview(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &AimPhase::transitionToOverview);

    Phase* overviewPhase = new OverviewPhase(this);
    changePhase(overviewPhase);
}
