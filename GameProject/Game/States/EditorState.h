#pragma once

#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>
#include "Engine/Collision/CollisionHandler.h"
#include "PauseState.h"

class EditorState : public State
{
private:
	//ImGui windows
	void mainWindow(EntityManager& entityManager);
	void entityWindow(EntityManager& entityManager);
	void levelWindow(EntityManager& entityManager);
	void playerWindow(EntityManager& entityManager);
	void editorWindow();

	std::string levelName;
	std::string currentItem;
	std::string currentModel;
	int currentEntity = -1;
	bool currentIsTarget;
	bool activeWindow[4];
	Entity camera;
	float camSpeed;
	FreeMove* freeMove;

	Level level;
	LevelParser levelParser;
	TargetManager* targetManager;
	ScoreManager scoreManager;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;

	void pauseGame(KeyEvent * ev);

public:
	EditorState();
	virtual ~EditorState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void render() override;
};