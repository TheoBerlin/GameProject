#include "GameLogic.h"

//#include <Engine/Rendering/Display.h>
//#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Game/GameLogic/Phases/OverviewPhase.h>
#include <Game/Components/CameraTransition.h>

GameLogic::GameLogic()
{
}

void GameLogic::init(Level& level)
{
	this->level = level;

	// Get preview camera component
	//Camera* previewCamera = Display::get().getRenderer().getActiveCamera();

	// Set up phase transition camera entity
	phaseTransitionEntity = level.entityManager->addTracedEntity("PhaseTransition");

	new CameraTransition(phaseTransitionEntity);

	Camera* camera = new Camera(phaseTransitionEntity);
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

void GameLogic::update(const float & dt)
{
	this->phase->update(dt);
}

void GameLogic::changePhaseCallback(PhaseChangeEvent * event)
{
	Phase* previousPhase = phase;

	phase = event->newPhase;

	delete previousPhase;
}
