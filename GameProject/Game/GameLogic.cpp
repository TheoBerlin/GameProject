#include "GameLogic.h"

#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Game/GameLogic/OverviewPhase.h>
#include <Game/Components/CameraTransition.h>

GameLogic::GameLogic()
{
}

void GameLogic::init(Level& level)
{
	this->level = level;

	// Set up phase transition camera entity
	phaseTransitionEntity = level.entityManager->addTracedEntity("PhaseTransition");

	new CameraTransition(phaseTransitionEntity);

	Camera* camera = new Camera(phaseTransitionEntity, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

	/*
		Start game in overview phase
	*/
	phase = new OverviewPhase(level, phaseTransitionEntity);

	// Handle phase changes
	EventBus::get().subscribe(this, &GameLogic::changePhaseCallback);
}

GameLogic::~GameLogic()
{
	EventBus::get().unsubscribe(this, &GameLogic::changePhaseCallback);

	delete phase;
}

void GameLogic::changePhaseCallback(PhaseChangeEvent * event)
{
	Phase* previousPhase = phase;

	phase = event->newPhase;

	delete previousPhase;
}
