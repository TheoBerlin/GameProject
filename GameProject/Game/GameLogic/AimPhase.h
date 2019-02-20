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
};
