#pragma once

#include <Engine/States/State.h>
#include <Game/Level/LevelParser.h>
#include <Game/GameLogic.h>
#include "Engine/Collision/CollisionHandler.h"
#include "PauseState.h"



class EditorState : public State
{
private:
#define NR_OF_WINDOWS 6

	//ImGui windows
	void mainWindow(EntityManager& entityManager);
	void entityWindow(EntityManager& entityManager);
	void levelWindow(EntityManager& entityManager);
	void playerWindow(EntityManager& entityManager);
	void wallWindow(EntityManager& entityManager);
	void lightWindow();
	void editorWindow();

	std::string levelName;
	std::string currentItem;
	std::string currentModel;
	int currentEntity = -1;
	bool currentIsTarget;
	std::string currentWall = "-1";
	std::string currentLight = "-1";
	bool activeWindow[NR_OF_WINDOWS];
	Entity camera;
	float camSpeed;
	FreeMove* freeMove;

	Level level;
	LevelParser levelParser;
	TargetManager* targetManager;
	ScoreManager scoreManager;
	GameLogic gameLogic;
	CollisionHandler collisionHandler;
	LevelStructure levelStructure;
	LightManager lightManager;

	void pauseGame(KeyEvent * ev);

public:
	EditorState();
	virtual ~EditorState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void render() override;
};