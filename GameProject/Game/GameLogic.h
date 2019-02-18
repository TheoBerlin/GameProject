#pragma once

#include <Engine/Components/FreeMove.h>
#include "Engine/Entity/EntityManager.h"
#include "Engine/Events/Events.h"
#include <Game/Components/PathVisualizer.h>
#include <Game/Level/Level.h>
#include <Game/GameLogic/Phase.h>
#include "glm/glm.hpp"

class GameLogic
{
public:
	GameLogic();
	void init(Level& level);
	~GameLogic();

	enum Phases { PHASE_OVERVIEW, PHASE_GUIDING, PHASE_REPLAY };
	void changePhase(Phases phase);

	void enterOverviewPhase(const glm::vec3 & cameraPos, const glm::vec3 & cameraDir);
	void enterGuidingPhase(const glm::vec3 & playerPos);
	void enterReplayPhase(const glm::vec3 & arrowPos);

	void leaveOverviewPhase();
	void leaveGuidingPhase();
	void leaveReplayPhase();

private:
	void changePhaseCallback(KeyEvent * ev);

	Level level;

	Phase* phase;
	Phases currentPhase;

	Entity* camera;
	Entity* player;

	EntityManager* em;
	TargetManager* targetManager;
};

