#include "OverviewPhase.h"

#include <Engine/Components/Camera.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>

OverviewPhase::OverviewPhase(const Level& level, const glm::vec3& cameraPos, const glm::vec3& cameraDir)
    :Phase(level)
{
	/*
		Create camera entity
	*/
	playerCamera = level.entityManager->addTracedEntity("PhaseOneCamera");
	playerCamera->getTransform()->setPosition(cameraPos);
	playerCamera->getTransform()->setForward(cameraDir);

	Camera* camera = new Camera(playerCamera, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

OverviewPhase::~OverviewPhase()
{
    level.entityManager->removeTracedEntity("PhaseOneCamera");
}
