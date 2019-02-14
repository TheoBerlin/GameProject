#pragma once

#include "Engine/Entity/EntityManager.h"
#include "glm/glm.hpp"
#include "Engine/Events/Events.h"

#include "Engine/GUI/GUIManager.h"

class GameLogic
{
public:
	GameLogic(EntityManager * em, GUIManager* guiManger);
	~GameLogic();

	enum Phases { PHASE_ONE, PHASE_TWO, PHASE_THREE };
	void changePhase(Phases phase);

	void enterPhaseOne(const glm::vec3 & cameraPos, const glm::vec3 & cameraDir);
	void enterPhaseTwo(const glm::vec3 & playerPos);

	void leavePhaseOne();
	void leavePhaseTwo();

private:
	void changePhaseCallback(KeyEvent * ev);

	Phases currentPhase;

	Entity* camera;
	Entity* player;

	EntityManager* em;
	GUIManager* guiManager;
};

