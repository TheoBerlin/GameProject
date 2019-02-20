#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/AimPhase.h>
#include <Game/GameLogic/OverviewPhase.h>
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
        Phase* replayPhase = new ReplayPhase(this);
        changePhase(replayPhase);
    }
}
