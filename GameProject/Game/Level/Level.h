#pragma once

#include <Engine/Entity/EntityManager.h>
#include <Game/GameLogic/TargetManager.h>
#include <Engine/Collision/CollisionHandler.h>

struct Level {
	EntityManager* entityManager;
	TargetManager* targetManager;
	CollisionHandler* collisionHandler;
};
