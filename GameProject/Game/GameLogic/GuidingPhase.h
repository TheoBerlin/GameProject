#pragma once

#include <Engine/Events/Events.h>
#include <Game/GameLogic/Phase.h>
#include <Game/Components/ArrowGuider.h>

// Phases GuidingPhase can transition from
class AimPhase;

class GuidingPhase : public Phase
{
public:
    GuidingPhase(AimPhase* other);

    Entity* getPlayerArrow() const;
    ArrowGuider* getArrowGuider() const;

private:
    void commonSetup();

    void handleKeyInput(KeyEvent* event);

    void transitionToReplay(CameraTransitionEvent* event);

    Entity* playerArrow;
    ArrowGuider* arrowGuider;

    // Created to avoid dragging the arrow around when transitioning
    Entity* transitionEntity;

    // TODO: These variables should be read from level during phase transitions
    const glm::vec3& playerPos = glm::vec3(1.0f, 1.0f, 12.0f);
    const glm::vec3& playerDir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
};
