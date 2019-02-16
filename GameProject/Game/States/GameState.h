#pragma once

#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>

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
	TargetManager* targetManager;
	GameLogic gameLogic;
};
