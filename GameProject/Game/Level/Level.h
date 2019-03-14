#pragma once

#include <Engine/Collision/CollisionHandler.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Particle/ParticleManager.h>
#include <Engine/GUI/GUI.h>
#include <Game/GameLogic/TargetManager.h>
#include <Game/GameLogic/ReplaySystem.h>
#include <Game/GameLogic/ScoreManager.h>
#include <Game/Level/LevelStructure.h>
#include <Engine/Rendering/Lighting/LightManager.h>
#include <glm/glm.hpp>

struct CameraSetting {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 offset;
	float FOV;
};

struct Player {
	CameraSetting oversightCamera;
	CameraSetting arrowCamera;
};

struct Level {
	bool isEditor = false;
	EntityManager* entityManager;
	TargetManager* targetManager;
	Player player;
	GUI* gui;
	CollisionHandler* collisionHandler;
	ReplaySystem* replaySystem;
	ScoreManager* scoreManager;
	LevelStructure* levelStructure;
	LightManager* lightManager;
};