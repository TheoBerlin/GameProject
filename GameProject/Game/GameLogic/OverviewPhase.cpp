#include "OverviewPhase.h"

#include <Engine/Components/Camera.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/GameLogic/AimPhase.h>
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
    overviewCamera->getTransform()->setPosition(cameraPos);
	overviewCamera->getTransform()->setForward(cameraDir);

	Camera* camera = new Camera(overviewCamera, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

    // Add oversight controller
    OversightController* camOversight = new OversightController(overviewCamera);

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
        Phase* guidingPhase = new AimPhase(this);
        changePhase(guidingPhase);
    }
}
