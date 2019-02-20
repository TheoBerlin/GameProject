#pragma once

#include <Engine/Events/Events.h>
#include <Game/GameLogic/Phase.h>

class OverviewPhase;

class ReplayPhase;

class GuidingPhase : public Phase
{
public:
    GuidingPhase(OverviewPhase* other);
    GuidingPhase(ReplayPhase* other);

private:
    void commonSetup();

    void handleKeyInput(KeyEvent* event);

    // TODO: These variables should be read from level during phase transitions
    const glm::vec3& playerPos = glm::vec3(1.0f, 1.0f, 12.0f);
    const glm::vec3& playerDir = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
};
