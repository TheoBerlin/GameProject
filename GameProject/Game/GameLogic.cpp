#include "GameLogic.h"

#include <Engine/Events/EventBus.h>
#include <Game/GameLogic/OverviewPhase.h>

GameLogic::GameLogic()
{
}

void GameLogic::init(Level& level)
{
	this->level = level;
	/*
		Start game in overview phase
	*/
	phase = new OverviewPhase(level);

	// Handle phase changes
	EventBus::get().subscribe(this, &GameLogic::changePhaseCallback);

	// Pass through key input to the current phase
	EventBus::get().subscribe(this, &GameLogic::handleKeyInput);
}

GameLogic::~GameLogic()
{
	EventBus::get().unsubscribe(this, &GameLogic::changePhaseCallback);
	EventBus::get().unsubscribe(this, &GameLogic::handleKeyInput);

	delete phase;
}

void GameLogic::changePhaseCallback(PhaseChangeEvent * event)
{
	Phase* previousPhase = phase;

	phase = event->newPhase;

	delete previousPhase;
}

void GameLogic::handleKeyInput(KeyEvent* event)
{
	phase->handleKeyInput(event);
}
