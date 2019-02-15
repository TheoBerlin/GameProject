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
	void updateLogic() override;
	void render() override;

	void emit(KeyEvent * evnt);

private:
	LevelParser levelParser;
	ParticleManager particleManger;
	ParticleEmitter emitter;
	ParticleEmitter emitter1;
	ParticleEmitter emitter2;
	ParticleEmitter emitter3;
	GameLogic gameLogic;
};