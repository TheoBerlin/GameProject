#include "ReplayPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Camera.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/GuidingPhase.h>

ReplayPhase::ReplayPhase(GuidingPhase* other)
    :Phase((Phase*)other)
{
    /*
		Create arrow entity
	*/
	replayArrow = level.entityManager->addTracedEntity("ArrowReplay");

	replayArrow->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

    replayArrow->getTransform()->setPosition(this->arrowPos);
	replayArrow->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	// Stop arrow guider and copy arrow path from guider to path treader
	Component* tmpPtr = player->getComponent("ArrowGuider");
    ArrowGuider* oldArrowGuider = dynamic_cast<ArrowGuider*>(tmpPtr);

    oldArrowGuider->stopGuiding();

    // Add path treader to arrow entity
    PathTreader* arrow = new PathTreader(replayArrow, oldArrowGuider->getPath());
    arrow->startTreading();

    // Add path visualizer for debugging
    pathVisualizer = new PathVisualizer(replayArrow, level.entityManager);
    pathVisualizer->addPath(oldArrowGuider->getPath());

    // Set up the player camera
    player->removeAllComponents();
    player->detachFromModel();

    // Create free camera
    Transform* playerTransform = player->getTransform();

	playerTransform->setPosition(glm::vec3(0.0f, 3.0f, 4.0f));
	playerTransform->setForward(glm::vec3(0.0f, -0.7f, -0.7f));
    playerTransform->resetRoll();

	Camera* camera = new Camera(player, "Camera");
	camera->init();

	FreeMove* freeMove = new FreeMove(player);

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

ReplayPhase::~ReplayPhase()
{
    pathVisualizer = nullptr;
    replayArrow = nullptr;
    pathTreader = nullptr;
}

Entity* ReplayPhase::getReplayArrow() const
{
    return replayArrow;
}

PathVisualizer* ReplayPhase::getPathVisualizer() const
{
    return pathVisualizer;
}

void ReplayPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_2) {
        Phase* guidingPhase = new GuidingPhase(this);
        changePhase(guidingPhase);
    }
}
