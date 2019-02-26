#pragma once

#include <Engine/Collision/CollisionHandler.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/GUI/GUI.h>
#include <Game/GameLogic/TargetManager.h>
#include <Game/GameLogic/ReplaySystem.h>
#include <glm/glm.hpp>

struct CameraSetting {
	glm::vec3 position;
	glm::vec3 direction;
};

struct Player {
	CameraSetting oversightCamera;
	CameraSetting arrowCamera;
	CameraSetting replayCamera;
};

struct Level {
	EntityManager* entityManager;
	TargetManager* targetManager;
	Player player;
	GUI* gui;
	CollisionHandler* collisionHandler;
	ReplaySystem* replaySystem;
};
