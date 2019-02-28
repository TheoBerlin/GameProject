#pragma once

#include <Engine/Components/FreeMove.h>
#include <Game/GameLogic/Phase.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/PathVisualizer.h>

class GuidingPhase;
class AimPhase;

class ReplayPhase : public Phase
{
public:
    ReplayPhase(GuidingPhase* guidingPhase);

    void update(const float& dt);

    Entity* getFreeCam() const;

    Entity* getReplayArrow() const;
    PathVisualizer* getPathVisualizer() const;

private:
    void handleKeyInput(KeyEvent* event);

    void transitionToAim(CameraTransitionEvent* event);

    Entity* freeCam;
    FreeMove* freeMove;

    Entity* replayArrow;
    PathTreader* pathTreader;
    PathVisualizer* pathVisualizer;

    // This should be read from the level struct in the future
    const glm::vec3 arrowPos = glm::vec3(1.0f, 1.0f, 16.0f);
};
