#pragma once

#include <Engine/Entity/EntityManager.h>
#include <Engine/GUI/GUI.h>
#include <Game/GameLogic/TargetManager.h>
#include <glm/glm.hpp>
#include <Engine/Collision/CollisionHandler.h>

struct CameraSetting {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 offset;
	float FOV;
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
};
