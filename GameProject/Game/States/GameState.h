#pragma once

#include "Engine/Particle/ParticleManager.h"
#include "Engine/Events/EventBus.h"
#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>
#include "Engine/Sound/SoundManager.h"
#include "Engine/Collision/CollisionHandler.h"
#include <Game/GameLogic/ScoreManager.h>
#include <Game/Level/LevelStructure.h>
#include <Engine/Rendering/Lighting/LightManager.h>

struct Level;

class GameState : public State
{
public:
	GameState(const Level& level);
	virtual ~GameState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic(const float dt) override;
	void render() override;

private:
	void pauseGame(PauseEvent * ev);
	void exitGame(ExitEvent* ev);

	bool hasSubscribedToPause, hasSubscribedToExit;

	std::string levelJSON;

	Level level;

	LevelParser levelParser;
	EntityManager* entityManager;
	TargetManager* targetManager;
	GameLogic gameLogic;
	CollisionHandler* collisionHandler;
	ReplaySystem replaySystem;
	ScoreManager scoreManager;
	LevelStructure* levelStructure;
	LightManager* lightManager;
	HelpGUI helpGUI;
};
