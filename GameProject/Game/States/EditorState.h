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
	//ImGui windows
	void mainWindow(EntityManager& entityManager);
	void entityWindow(EntityManager& entityManager);
	void levelWindow();
	void cameraWindow();

	std::string levelName;
	std::string currentItem;
	std::string currentModel;
	int currentEntity = -1;
	bool currentIsTarget;
	bool activeWindow[3];
	Entity camera;
	float camSpeed;
	FreeMove* freeMove;

	Level level;
	LevelParser levelParser;
	TargetManager* targetManager;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;
};