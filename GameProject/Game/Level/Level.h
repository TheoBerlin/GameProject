#pragma once

#include <Engine/Entity/EntityManager.h>
#include <Game/GameLogic/TargetManager.h>

struct Level {
	EntityManager* entityManager;
	TargetManager* targetManager;
};
