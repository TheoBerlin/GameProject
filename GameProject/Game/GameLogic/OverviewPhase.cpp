#include "OverviewPhase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/GameLogic/AimPhase.h>
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

    glm::vec3 camOffset = { 0.0f, 0.5f, -2.0f };

    overviewCamera->getTransform()->setPosition(level.player.oversightCamera.position - camOffset);
	overviewCamera->getTransform()->setForward(level.player.oversightCamera.direction);

	Camera* camera = new Camera(overviewCamera, "Camera", { 0.0f, 0.5f, -2.0f });
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
        glm::vec3 newPos = level.player.arrowCamera.position;
        glm::vec3 newForward = playerArrow->getTransform()->getForward();
        float transitionLength = 2.0f;

        glm::vec3 currentPosition = overviewCamera->getTransform()->getPosition();
        glm::vec3 currentForward = overviewCamera->getTransform()->getForward();

        transitionEntity->getTransform()->setPosition(currentPosition);
        transitionEntity->getTransform()->setForward(currentForward);

        transitionComponent->setDestination(newPos, newForward, transitionLength);

        Display::get().getRenderer().setActiveCamera(transitionCam);

        EventBus::get().subscribe(this, &OverviewPhase::transitionToAim);
    }
}

void OverviewPhase::transitionToAim(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &OverviewPhase::transitionToAim);

    Phase* guidingPhase = new AimPhase(this);
    changePhase(guidingPhase);
}
