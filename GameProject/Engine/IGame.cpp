#include "IGame.h"
#include "../Utils/Timer.h"
#include "Config.h"
#include "Rendering/Display.h"

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
	Display& display = Display::get();
	
	float totalTime = 0.0f;
	float dt = 0.0f;
	Timer dtTimer;
	while (this->isRunning && !this->stateManager.isEmpty() && display.isOpen())
	{
		dtTimer.restart();
		
		display.startFrame();

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

		display.endFrame();
	}
}
