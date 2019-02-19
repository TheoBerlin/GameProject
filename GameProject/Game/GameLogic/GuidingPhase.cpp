#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/GameLogic/ReplayPhase.h>
#include <Utils/Logger.h>

GuidingPhase::GuidingPhase(OverviewPhase* other)
    :Phase((Phase*)other)
{
    commonSetup();
}

GuidingPhase::GuidingPhase(ReplayPhase* other)
    :Phase((Phase*)other)
{

    // Remove path visualizers
    PathVisualizer* pathVisualizer = other->getPathVisualizer();

    if (pathVisualizer) {
        other->getPathVisualizer()->removeVisualizers();
    }

    // Remove replay arrow
    Entity* replayArrow = other->getReplayArrow();

    level.entityManager->removeTracedEntity(replayArrow->getName());

    commonSetup();
}

GuidingPhase::~GuidingPhase()
{
}

void GuidingPhase::commonSetup()
{
    player->removeAllComponents();

	/*
		Create arrow entity
	*/
	Model * model = ModelLoader::loadModel("./Game/assets/Arrow.fbx");

    Transform* playerTransform = player->getTransform();

    playerTransform->setForward(playerDir);
    playerTransform->resetRoll();
	playerTransform->setPosition(playerPos);
	playerTransform->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	player->setModel(model);

	/*
		Add camera to arrow entity
	*/
	Camera* camera = new Camera(player, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	/*
		Add arrowguider to entity
	*/
	ArrowGuider* arrow = new ArrowGuider(player, 2.0f);
	arrow->startGuiding();

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

void GuidingPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_1) {
        Phase* overviewPhase = new OverviewPhase(this);
        changePhase(overviewPhase);
    } else if (event->key == GLFW_KEY_3) {
        Phase* replayPhase = new ReplayPhase(this);
        changePhase(replayPhase);
    }
}

