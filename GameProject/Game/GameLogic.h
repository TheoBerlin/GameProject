#pragma once

#include <Engine/Components/FreeMove.h>
#include "Engine/Entity/EntityManager.h"
#include "Engine/Events/Events.h"
#include <Game/Components/PathVisualizer.h>
#include <Game/Level/Level.h>
#include <Game/GameLogic/Phase.h>
#include "glm/glm.hpp"

class CollisionHandler;

class GameLogic
{
public:
	GameLogic();
	void init(Level& level);
	~GameLogic();

	virtual void update(const float& dt);

private:
	void changePhaseCallback(PhaseChangeEvent * event);

	// Used for camera transitions
	Entity* phaseTransitionEntity;

	Level level;

	Phase* phase;
};
