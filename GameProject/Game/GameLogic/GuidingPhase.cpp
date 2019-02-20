#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/CameraTransition.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Game/GameLogic/ReplayPhase.h>
#include <Utils/Logger.h>

GuidingPhase::GuidingPhase(AimPhase* aimPhase)
    :Phase((Phase*)aimPhase)
{
    playerArrow = aimPhase->getPlayerArrow();

    // Start guiding the arrow
    arrowGuider = aimPhase->getArrowGuider();
    arrowGuider->startGuiding();

    level.targetManager->resetTargets();
}

Entity* GuidingPhase::getPlayerArrow() const
{
    return playerArrow;
}

ArrowGuider* GuidingPhase::getArrowGuider() const
{
    return arrowGuider;
}

void GuidingPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_3) {
        // Create transition camera
        transitionEntity = level.entityManager->addTracedEntity("TransitionCamera");

        // Begin camera transition to the replay freecam
        glm::vec3 newPos = level.player.replayCamera.position;
        glm::vec3 newForward = level.player.replayCamera.direction;
        float transitionLength = 2.0f;

        CameraTransition* camTransition = new CameraTransition(transitionEntity, newPos, newForward, transitionLength);

        EventBus::get().subscribe(this, &GuidingPhase::transitionToReplay);
    }
}

void GuidingPhase::transitionToReplay(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &GuidingPhase::transitionToReplay);

    level.entityManager->removeTracedEntity(transitionEntity->getName());

    Phase* guidingPhase = new ReplayPhase(this);
    changePhase(guidingPhase);
}
