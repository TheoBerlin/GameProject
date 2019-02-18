#pragma once

#include <Engine/Components/FreeMove.h>
#include "Engine/Entity/EntityManager.h"
#include "Engine/Events/Events.h"
#include <Game/Components/PathVisualizer.h>
#include "glm/glm.hpp"

#include "Engine/GUI/GUI.h"
#include "Engine/GUI/Panel.h"

class GameLogic
{
public:
	GameLogic(EntityManager * em, GUI* gui);
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

	Phases currentPhase;

	Entity* camera;
	Entity* player;

	EntityManager* em;
	GUI* gui;
};

