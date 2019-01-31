#include "IGame.h"
#include "../Utils/Timer.h"

IGame::IGame()
{
}

IGame::~IGame()
{
}

void IGame::start()
{
	this->isRunning = true;
	gameLoop();
}

StateManager& IGame::getStateManager()
{
	return this->stateManager;
}

void IGame::gameLoop()
{
	const unsigned int FRAMES_PER_SECOND = 30;

	float totalTime = 0.0f;
	float dt = 0.0f;
	Timer dtTimer;
	while (this->isRunning && !this->stateManager.isEmpty())
	{
		dtTimer.restart();

		// Update state
		this->stateManager.update(dt);
		onUpdate(dt);

		// Update logic in a fixed interval
		if (totalTime >= 1.0f / (float)FRAMES_PER_SECOND) {
			this->stateManager.updateLogic();
			onUpdateLogic();
			totalTime = 0.0f;
		}

		// Render state
		this->stateManager.render();
		onRender();

		// Restart dtTimer
		dtTimer.stop();
		dt = dtTimer.getTime();
		totalTime += dt;
	}
}
