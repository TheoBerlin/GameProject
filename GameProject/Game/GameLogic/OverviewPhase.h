#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Events/Events.h>
#include <Game/GameLogic/Phase.h>

class AimPhase;

class OverviewPhase : public Phase
{
public:
    OverviewPhase(AimPhase* aimPhase);
    OverviewPhase(const Level& level);

    Entity* getOverviewCamera() const;

private:
    void commonSetup();

    void handleKeyInput(KeyEvent* event);

    Entity* overviewCamera;

    // This should be read from the level struct in the future
    const glm::vec3 cameraPos = glm::vec3(-10.0f, 20.0f, 10.0f);
    const glm::vec3& cameraDir = glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f));
};
