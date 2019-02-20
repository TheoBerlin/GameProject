#include "OverviewPhase.h"

#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Events/Events.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Game/Components/CameraTransition.h>
#include <Game/Components/OversightController.h>
#include <Utils/Logger.h>

OverviewPhase::OverviewPhase(AimPhase* aimPhase)
    :Phase((Phase*)aimPhase)
{
    // Remove arrow
    level.entityManager->removeTracedEntity(aimPhase->getPlayerArrow()->getName());

    commonSetup();
}

OverviewPhase::OverviewPhase(const Level& level)
    :Phase(level)
{
	/*
		Create camera entity
	*/
    commonSetup();
}

Entity* OverviewPhase::getOverviewCamera() const
{
    return overviewCamera;
}

void OverviewPhase::commonSetup()
{
    overviewCamera = level.entityManager->addTracedEntity("OverviewCamera");

    overviewCamera->getTransform()->setPosition(level.player.oversightCamera.position);
	overviewCamera->getTransform()->setForward(level.player.oversightCamera.direction);

	Camera* camera = new Camera(overviewCamera, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

    // Add oversight controller
    oversightControl = new OversightController(overviewCamera);

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

void OverviewPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_2) {
        // Remove oversight control
        overviewCamera->removeComponent(oversightControl->getName());

        // Begin camera transition to the arrow
        glm::vec3 newPos = level.player.arrowCamera.position;
        glm::vec3 newForward = level.player.arrowCamera.direction;
        float transitionLength = 2.0f;

        CameraTransition* camTransition = new CameraTransition(overviewCamera, newPos, newForward, transitionLength);

        EventBus::get().subscribe(this, &OverviewPhase::transitionToAim);
    }
}

void OverviewPhase::transitionToAim(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &OverviewPhase::transitionToAim);

    Phase* guidingPhase = new AimPhase(this);
    changePhase(guidingPhase);
}
