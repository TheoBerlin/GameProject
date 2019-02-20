#pragma once

#include "../../Engine/States/State.h"
#include "../../Engine/Level/LevelParser.h"
#include "Engine/Particle/ParticleManager.h"
#include "Engine/Events/EventBus.h"
#include "Engine/Events/Events.h"
#include "../../Game/GameLogic.h"

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

	void emit(KeyEvent * evnt);

private:
	LevelParser levelParser;
	ParticleEmitter emitter;
	GameLogic gameLogic;
};