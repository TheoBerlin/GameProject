#pragma once

#include <Engine/Events/Events.h>
#include <Game/GameLogic/Phase.h>

class GuidingPhase;

class OverviewPhase : public Phase
{
public:
    OverviewPhase(GuidingPhase* other);
    OverviewPhase(const Level& level);

    ~OverviewPhase();

private:
    void commonSetup();
    void handleKeyInput(KeyEvent* event);

    // This should be read from the level struct in the future
    const glm::vec3 cameraPos = glm::vec3(-10.0f, 20.0f, 10.0f);
    const glm::vec3& cameraDir = glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f));
};
