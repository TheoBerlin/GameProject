#pragma once

#include <Game/GameLogic/Phase.h>
#include <Game/Components/ArrowGuider.h>

class OverviewPhase;
class ReplayPhase;

class AimPhase : public Phase
{
public:
    AimPhase(OverviewPhase* overviewPhase);
    AimPhase(ReplayPhase* replayPhase);

    Entity* getPlayerArrow() const;
    ArrowGuider* getArrowGuider() const;

private:
    void commonSetup();

    void handleMouseClick(MouseClickEvent* event);
    void handleKeyInput(KeyEvent* event);

    Entity* playerArrow;
    ArrowGuider* arrowGuider;

    // TODO: These variables should be read from level during phase transitions
    const glm::vec3& playerPos = glm::vec3(1.0f, 1.0f, 12.0f);
    const glm::vec3& playerDir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
};
