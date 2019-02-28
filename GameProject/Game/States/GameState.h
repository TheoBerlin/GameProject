#pragma once

#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>
#include "Engine/Collision/CollisionHandler.h"
#include <Game/GameLogic/ScoreManager.h>

class GameState : public State
{
public:
	GameState(const std::string& levelJSON);
	virtual ~GameState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic(const float dt) override;
	void render() override;

private:
	void pauseGame(KeyEvent * ev);

	bool hasSubscribedToPause;

	LevelParser levelParser;
	TargetManager* targetManager;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;
	ReplaySystem replaySystem;
	ScoreManager scoreManager;
};