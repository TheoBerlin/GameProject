#pragma once

#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>
#include "Engine/Collision/CollisionHandler.h"

class EditorState : public State
{
public:
	EditorState();
	virtual ~EditorState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic(const float dt) override;
	void render() override;

private:
	void entityWindow(EntityManager& entityManager);
	std::string currentItem;
	int currentEntity = -1;
	Entity camera;

	LevelParser levelParser;
	TargetManager* targetManager;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;
};