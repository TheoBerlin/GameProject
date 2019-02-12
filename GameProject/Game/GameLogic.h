#pragma once

#include "Engine/Entity/EntityManager.h"

class GameLogic
{
public:
	GameLogic(EntityManager * em);
	~GameLogic();

	void changePhase(unsigned phase);

private:
	unsigned currentPhase;

	Entity* camera;
	Entity* player;

	EntityManager* em;
};

