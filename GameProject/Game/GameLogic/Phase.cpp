#include "Phase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Events/Events.h>

Phase::Phase(const Level& level)
{
    this->level = level;
}

Phase::Phase(Phase* other)
{
    this->level = other->level;
}

void Phase::handleKeyInput(KeyEvent* event)
{
}

void Phase::handleMouseClick(MouseClickEvent* event)
{
}

void Phase::changePhase(Phase* newPhase)
{
    EventBus::get().publish(&PhaseChangeEvent(newPhase));
}
