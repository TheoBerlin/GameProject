#include "Phase.h"

#include <Engine/Events/EventBus.h>
#include <Engine/Events/Events.h>

Phase::Phase(const Level& level)
{
    this->level = level;
}

Phase::Phase(Phase* other)
{
    LOG_INFO("Copying phase");
    this->level = other->level;
    this->player = other->player;
}

Phase::~Phase()
{
    LOG_INFO("Phase destructor");
	//level.entityManager->removeTracedEntity("Player");
    //delete player;
}

void Phase::handleKeyInput(KeyEvent* event)
{
}

void Phase::changePhase(Phase* newPhase)
{
    LOG_INFO("Publishing phase event");
    EventBus::get().publish(&PhaseChangeEvent(newPhase));
}
