#pragma once

#include "States/StateManager.h"

class IGame
{
public:
	IGame();
	virtual ~IGame();

	void start();
	virtual void onUpdate(float dt) {};
	virtual void onUpdateLogic() {};
	virtual void onRender() {};

private:
	void gameLoop();
	
	bool isRunning;
	StateManager stateManager;
};