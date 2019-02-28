#pragma once

#include <Engine/Components/FreeMove.h>
#include <Game/GameLogic/Phase.h>
#include <Game/Components/PathTreader.h>
#include <Game/Components/PathVisualizer.h>

class GuidingPhase;
class AimPhase;
class Panel;

class ReplayPhase : public Phase
{
public:
    ReplayPhase(GuidingPhase* guidingPhase);
    ReplayPhase(AimPhase* aimPhase);

	void update(const float& dt);

    Entity* getFreeCam() const;

    Entity* getReplayArrow() const;
    PathVisualizer* getPathVisualizer() const;

private:
    void handleKeyInput(KeyEvent* event);

    void transitionToAim(CameraTransitionEvent* event);

	void activateGui();
	void guiCallback();

    Entity* freeCam;
    FreeMove* freeMove;

    Entity* replayArrow;
    PathTreader* pathTreader;
    PathVisualizer* pathVisualizer;

	// Gui stuff
	float timeLeft;
	bool showGui;
	bool minimized;
	Panel* smallPanel;
	Panel* bigPanel;


    // This should be read from the level struct in the future
    const glm::vec3 arrowPos = glm::vec3(1.0f, 1.0f, 16.0f);
};
