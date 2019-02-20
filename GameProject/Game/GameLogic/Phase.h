#pragma once

#include <Game/Level/Level.h>
#include <Engine/Events/Events.h>

class Phase
{
public:
    Phase(const Level& level);
    Phase(Phase* other);

    virtual void handleKeyInput(KeyEvent* event);
    virtual void handleMouseClick(MouseClickEvent* event);

protected:
    void changePhase(Phase* newPhase);

    Level level;
};
