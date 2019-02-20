#include "AimPhase.h"

#include <Engine/Components/Camera.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/GameLogic/GuidingPhase.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/GameLogic/ReplayPhase.h>
#include <GLFW/glfw3.h>

AimPhase::AimPhase(OverviewPhase* overviewPhase)
    :Phase((Phase*)overviewPhase)
{
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

    commonSetup();
}

Entity* AimPhase::getPlayerArrow() const
{
    return playerArrow;
}

ArrowGuider* AimPhase::getArrowGuider() const
{
    return arrowGuider;
}

void AimPhase::commonSetup()
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

	/*
		Add camera to arrow entity
	*/
	Camera* camera = new Camera(playerArrow, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	/*
		Add arrowguider to entity
	*/
	arrowGuider = new ArrowGuider(playerArrow, 2.0f);
	arrowGuider->startAiming();

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

void AimPhase::handleMouseClick(MouseClickEvent* event)
{
    // Change phase to guiding phase when the left mouse button has been released
    if (event->action == GLFW_RELEASE && event->button == GLFW_MOUSE_BUTTON_LEFT) {
        Phase* newPhase = new GuidingPhase(this);

        changePhase(newPhase);
    }
}

void AimPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_1) {
        Phase* overviewPhase = new OverviewPhase(this);
        changePhase(overviewPhase);
    }
}
