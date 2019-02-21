#pragma once

#include <Engine/Entity/EntityManager.h>
#include <Game/GameLogic/TargetManager.h>
#include <Engine/Collision/CollisionHandler.h>
#include <Engine/GUI/GUI.h>

struct Level {
	EntityManager* entityManager;
	TargetManager* targetManager;
	GUI* gui;
	CollisionHandler* collisionHandler;
};
