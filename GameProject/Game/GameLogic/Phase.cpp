#include "Phase.h"

#include <Engine/Events/EventBus.h>

Phase::Phase(const Level& level, Entity* transitionEntity)
    :level(level),
    transitionEntity(transitionEntity)
{
    this->transitionComponent = dynamic_cast<CameraTransition*>(transitionEntity->getComponent("CameraTransition"));
    this->transitionCam = dynamic_cast<Camera*>(transitionEntity->getComponent("Camera"));
}

Phase::Phase(Phase* other)
    :level(other->level),
    transitionEntity(other->transitionEntity),
    transitionComponent(other->transitionComponent),
    transitionCam(other->transitionCam)
{
}

void Phase::changePhase(Phase* newPhase)
{
    EventBus::get().publish(&PhaseChangeEvent(newPhase));
}
