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

private:
	void changePhaseCallback(PhaseChangeEvent * event);

	// Pass through input to the current phase
	void handleKeyInput(KeyEvent* event);
	void handleMouseClick(MouseClickEvent* event);

	Level level;

	Phase* phase;
};

