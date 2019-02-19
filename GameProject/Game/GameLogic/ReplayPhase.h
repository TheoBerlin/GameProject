#pragma once

#include <Game/GameLogic/Phase.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/PathVisualizer.h>

class GuidingPhase;

class ReplayPhase : public Phase
{
public:
    ReplayPhase(GuidingPhase* other);

    ~ReplayPhase();

    Entity* getReplayArrow() const;
    PathVisualizer* getPathVisualizer() const;

private:
    void handleKeyInput(KeyEvent* event);

    PathTreader* pathTreader;
    Entity* replayArrow;
    PathVisualizer* pathVisualizer;

    // This should be read from the level struct in the future
    const glm::vec3 arrowPos = glm::vec3(1.0f, 1.0f, 16.0f);
};
