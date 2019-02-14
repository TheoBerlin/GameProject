#pragma once

#include "../../Engine/States/State.h"
#include "../../Engine/Level/LevelParser.h"
#include "../../Game/GameLogic.h"
#include "Engine/Collision/CollisionHandler.h"

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

private:
	LevelParser levelParser;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;
};