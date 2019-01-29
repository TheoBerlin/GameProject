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

void IGame::gameLoop()
{
	const unsigned int FRAMES_PER_SECOND = 60;

	float totalTime = 0.0f;
	float dt = 0.0f;
	Timer dtTimer;
	while (this->isRunning)
	{
		dtTimer.restart();

		if (this->stateManager.isEmpty() == false)
		{
			// Update state
			this->stateManager.update(dt);
			onUpdate(dt);

			// Update logic in a fixed interval
			if (totalTime >= 1.0f / (float)FRAMES_PER_SECOND) {
				totalTime = 0;
				this->stateManager.updateLogic();
				onUpdateLogic();
			}

			// Render state
			this->stateManager.render();
			onRender();
		}

		// Restart dtTimer
		dtTimer.stop();
		dt = dtTimer.getTime();
		totalTime += dt;
	}
}
