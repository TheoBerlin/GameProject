#pragma once

#include "States/StateManager.h"

class IGame
{
public:
	IGame();
	virtual ~IGame();

	void start();
	virtual void onUpdate(float dt) {};
	virtual void onUpdateLogic(const float dt) {};
	virtual void onRender() {};

	StateManager& getStateManager();

private:
	void gameLoop();
	
	bool isRunning;
	StateManager stateManager;
};