#include "GameLogic.h"

#include "../Engine/Rendering/Display.h"
#include "../Engine/Rendering/Renderer.h"
#include "../Engine/Components/Component.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/AssetManagement/ModelLoader.h"
#include "Engine/Events/EventBus.h"
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathTreader.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/GameLogic/GuidingPhase.h>
#include <Utils/Logger.h>

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

	// Pass through input to the current phase
	EventBus::get().subscribe(this, &GameLogic::handleKeyInput);
	EventBus::get().subscribe(this, &GameLogic::handleMouseClick);
}

GameLogic::~GameLogic()
{
	EventBus::get().unsubscribe(this, &GameLogic::changePhaseCallback);
	EventBus::get().unsubscribe(this, &GameLogic::handleKeyInput);
	EventBus::get().unsubscribe(this, &GameLogic::handleMouseClick);

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

void GameLogic::handleMouseClick(MouseClickEvent* event)
{
	phase->handleMouseClick(event);
}
