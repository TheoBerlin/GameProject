#pragma once

#include <Game/Level/Level.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Events/Events.h>

class Phase
{
public:
    Phase(const Level& level);
    Phase(Phase* other);

    virtual void handleKeyInput(KeyEvent* event);

protected:
    void changePhase(Phase* newPhase);

    Entity* player;

    Level level;
};
