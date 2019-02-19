#include "OverviewPhase.h"

#include <Engine/Components/Camera.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/GameLogic/GuidingPhase.h>
#include <Utils/Logger.h>

OverviewPhase::OverviewPhase(GuidingPhase* other)
    :Phase((Phase*)other)
{
    player->removeAllComponents();
    player->detachFromModel();

    commonSetup();
}

OverviewPhase::OverviewPhase(const Level& level)
    :Phase(level)
{
	/*
		Create camera entity
	*/
    player = level.entityManager->addTracedEntity("Player");

    commonSetup();
}

OverviewPhase::~OverviewPhase()
{
}

void OverviewPhase::commonSetup()
{
    player->getTransform()->setPosition(cameraPos);
	player->getTransform()->setForward(cameraDir);

	Camera* camera = new Camera(player, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

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
        Phase* guidingPhase = new GuidingPhase(this);
        changePhase(guidingPhase);
    }
}
