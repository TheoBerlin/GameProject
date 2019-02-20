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
#include <Game/GameLogic/AimPhase.h>

ReplayPhase::ReplayPhase(GuidingPhase* guidingPhase)
    :Phase((Phase*)guidingPhase)
{
    /*
		Create replay arrow
	*/
	replayArrow = level.entityManager->addTracedEntity("ArrowReplay");

	replayArrow->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

    replayArrow->getTransform()->setPosition(this->arrowPos);
	replayArrow->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	// Stop arrow guider and copy arrow path from guider to path treader
    ArrowGuider* oldArrowGuider = guidingPhase->getArrowGuider();
    oldArrowGuider->stopGuiding();

    PathTreader* arrow = new PathTreader(replayArrow, oldArrowGuider->getPath());
    arrow->startTreading();

    // Add path visualizer for debugging
    pathVisualizer = new PathVisualizer(replayArrow, level.entityManager);
    pathVisualizer->addPath(oldArrowGuider->getPath());

    // Remove old arrow entity
    Entity* oldArrow = guidingPhase->getPlayerArrow();
    level.entityManager->removeTracedEntity(oldArrow->getName());

    // Set up the player camera
    freeCam = level.entityManager->addTracedEntity("FreeCam");

    Transform* camTransform = freeCam->getTransform();

	camTransform->setPosition(glm::vec3(0.0f, 3.0f, 4.0f));
	camTransform->setForward(glm::vec3(0.0f, -0.7f, -0.7f));
    camTransform->resetRoll();

	Camera* camera = new Camera(freeCam, "Camera");
	camera->init();

	FreeMove* freeMove = new FreeMove(freeCam);

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

Entity* ReplayPhase::getFreeCam() const
{
    return freeCam;
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
        Phase* guidingPhase = new AimPhase(this);
        changePhase(guidingPhase);
    }
}
