#include "AimPhase.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/GameLogic/GuidingPhase.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/GameLogic/ReplayPhase.h>
#include <Engine/Components/PlayerCollision.h>
#include <GLFW/glfw3.h>

#include "Engine/Config.h"
#include "Engine/Imgui/imgui.h"

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
#ifdef IMGUI
	ArrowGuider* arrowGuider = this->getArrowGuider();

	if (ImGui::Begin("Arrow Config")) {
		ImGui::DragFloat("Speed:", &this->arrowSpeed, 0.2f);
		ImGui::DragFloat("Max turnspeed:", &this->maxTurnSpeed, 0.5f);

		ImGui::End();
	}

#endif
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



	/*
		Add arrowguider to entity
	*/
	arrowGuider = new ArrowGuider(playerArrow, arrowCamSettings.offset, arrowCamSettings.FOV, 3.0f);
	arrowCam = new Camera(playerArrow, "Camera", camOffset);

	arrowCam->setFOV(arrowCamSettings.FOV);
	arrowCam->init();

	arrowGuider->startAiming();

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(arrowCam);

    EventBus::get().subscribe(this, &AimPhase::handleKeyInput);
    EventBus::get().subscribe(this, &AimPhase::handleMouseClick);

#ifdef IMGUI
	/*
	Set up for configuring arrow data
	*/
	this->configureData = false;
	this->arrowSpeed = this->arrowGuider->getMovementSpeed();
	this->maxTurnSpeed = this->arrowGuider->getMaxTurnSpeed();

#endif
}

void AimPhase::handleMouseClick(MouseClickEvent* event)
{
#ifdef IMGUI
    // Change phase to guiding phase when the left mouse button has been released
    if (event->action == GLFW_RELEASE && event->button == GLFW_MOUSE_BUTTON_LEFT && !this->configureData) {
        EventBus::get().unsubscribe(this, &AimPhase::handleKeyInput);
        EventBus::get().unsubscribe(this, &AimPhase::handleMouseClick);

		// Apply configured values
		this->arrowGuider->setMovementSpeed(this->arrowSpeed);
		this->arrowGuider->setMaxTurnSpeed(this->maxTurnSpeed);

        Phase* newPhase = new GuidingPhase(this);

        changePhase(newPhase);
    }
#else
	// Change phase to guiding phase when the left mouse button has been released
	if (event->action == GLFW_RELEASE && event->button == GLFW_MOUSE_BUTTON_LEFT) {
		EventBus::get().unsubscribe(this, &AimPhase::handleKeyInput);
		EventBus::get().unsubscribe(this, &AimPhase::handleMouseClick);

		Phase* newPhase = new GuidingPhase(this);

		changePhase(newPhase);
	}
#endif
}

void AimPhase::handleKeyInput(KeyEvent* event)
{
#ifdef IMGUI
	if (event->key == GLFW_KEY_C && event->action == GLFW_PRESS) {
		if (this->configureData) {
			// Lock cursor
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			this->getArrowGuider()->startAiming();
		}
		else {

			// Unlock cursor
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			this->getArrowGuider()->stopAiming();
		}

		this->configureData = !this->configureData;
	}

	if (event->key == GLFW_KEY_1 && !this->configureData) {
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
#else

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
#endif
}

void AimPhase::transitionToOverview(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &AimPhase::transitionToOverview);

    Phase* overviewPhase = new OverviewPhase(this);
    changePhase(overviewPhase);
}
