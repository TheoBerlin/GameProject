#pragma once

#include "Engine/Particle/ParticleManager.h"
#include "Engine/Events/EventBus.h"
#include "Engine/Events/Events.h"
#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>
#include "Engine/Collision/CollisionHandler.h"

class GameState : public State
{
public:
	GameState();
	virtual ~GameState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic(const float dt) override;
	void render() override;
private:
	LevelParser levelParser;
	ParticleEmitter emitter;
	TargetManager* targetManager;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;
	ReplaySystem replaySystem;
};