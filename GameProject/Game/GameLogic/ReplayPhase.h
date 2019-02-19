#pragma once

#include <Game/GameLogic/Phase.h>
#include <Game/Components/PathTreader.h>

class GuidingPhase;

class ReplayPhase : public Phase
{
public:
    ReplayPhase(GuidingPhase* other);

    ~ReplayPhase();

private:
    void handleKeyInput(KeyEvent* event);

    PathTreader* pathTreader;

    // This should be read from the level struct in the future
    const glm::vec3 arrowPos = glm::vec3(1.0f, 1.0f, 16.0f);
};
