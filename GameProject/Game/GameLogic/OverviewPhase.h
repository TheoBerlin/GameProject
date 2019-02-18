#pragma once

#include <Game/GameLogic/Phase.h>

class OverviewPhase : public Phase
{
public:
    OverviewPhase(const Level& level, const glm::vec3& cameraPos, const glm::vec3& cameraDir);
    ~OverviewPhase();

private:
};
