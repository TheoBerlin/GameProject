#include "OverviewPhase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Engine/Components/PlayerCollision.h>
#include <Game/Components/OversightController.h>

OverviewPhase::OverviewPhase(AimPhase* aimPhase)
    :Phase((Phase*)aimPhase)
{
    // Remove all arrow components
    playerArrow = aimPhase->getPlayerArrow();

    playerArrow->removeAllComponents();

    commonSetup();
}

OverviewPhase::OverviewPhase(const Level& level, Entity* transitionEntity)
    :Phase(level, transitionEntity)
{
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

	new PlayerCollision(playerArrow);

    commonSetup();
}

Entity* OverviewPhase::getOverviewCamera() const
{
    return overviewCamera;
}

Entity* OverviewPhase::getPlayerArrow() const
{
    return playerArrow;
}

void OverviewPhase::commonSetup()
{
    // Create oversight camera
    overviewCamera = level.entityManager->addTracedEntity("OverviewCamera");

    CameraSetting camSetting = level.player.oversightCamera;

    overviewCamera->getTransform()->setPosition(camSetting.position);
	overviewCamera->getTransform()->setForward(camSetting.direction);

	Camera* camera = new Camera(overviewCamera, "Camera", camSetting.offset);
    camera->setFOV(level.player.oversightCamera.FOV);
	camera->init();

    // Add oversight controller
    overviewControl = new OversightController(overviewCamera);

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);

    EventBus::get().subscribe(this, &OverviewPhase::handleKeyInput);
}

void OverviewPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_2) {
        EventBus::get().unsubscribe(this, &OverviewPhase::handleKeyInput);

        // Remove oversight control
        overviewCamera->removeComponent(overviewControl->getName());

        // Begin camera transition to the arrow
        Transform* overviewTransform = overviewCamera->getTransform();

        // Current camera settings
        CameraSetting currentCamSettings = level.player.oversightCamera;

        currentCamSettings.direction = overviewTransform->getForward();
        currentCamSettings.position = overviewTransform->getPosition();

        // New camera settings
        CameraSetting newCamSettings = level.player.arrowCamera;

        newCamSettings.direction = playerArrow->getTransform()->getForward();

        this->setupTransition(currentCamSettings, newCamSettings);

        EventBus::get().subscribe(this, &OverviewPhase::transitionToAim);
    }
}

void OverviewPhase::transitionToAim(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &OverviewPhase::transitionToAim);

    Phase* guidingPhase = new AimPhase(this);
    changePhase(guidingPhase);
}
