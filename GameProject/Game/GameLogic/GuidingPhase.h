#pragma once

#include <Game/GameLogic/Phase.h>

class GuidingPhase : public Phase
{
public:
    GuidingPhase(const Level& level, const glm::vec3& playerPos);
    GuidingPhase(Phase& other);
    ~GuidingPhase();
};
