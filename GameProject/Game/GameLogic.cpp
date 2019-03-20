#include "GameLogic.h"

#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Components/Camera.h>
#include <Engine/Events/EventBus.h>
#include <Game/GameLogic/Phases/OverviewPhase.h>
#include <Game/Components/CameraTransition.h>

#include <Utils/Logger.h>

GameLogic::GameLogic()
	:phase(nullptr)
{
}

void GameLogic::init(Level& level)
{
	this->level = level;

	// Get level preview camera component
	Camera* oldCam = Display::get().getRenderer().getActiveCamera();

	Entity* oldCamEntity = oldCam->getHost();

	Transform* oldCamTransform = oldCamEntity->getTransform();

	// Set up phase transition camera entity
	Entity* phaseTransitionEntity = level.entityManager->addTracedEntity("PhaseTransition");

	// Transition to overview camera
	CameraTransition* camTransition = new CameraTransition(phaseTransitionEntity);

	Transform* newCamTransform = phaseTransitionEntity->getTransform();

	newCamTransform->setPosition(oldCamTransform->getPosition());
	newCamTransform->setForward(oldCamTransform->getForward());

	oldCam->setHost(phaseTransitionEntity);

	// Remove old camera entity
	level.entityManager->removeTracedEntity(oldCamEntity->getName());

	/*
		Start game in overview phase
	*/
	phase = new OverviewPhase(level, phaseTransitionEntity);

	Display::get().getRenderer().initInstancing();

	// Handle phase changes
	EventBus::get().subscribe(this, &GameLogic::changePhase);
}

GameLogic::~GameLogic()
{
	EventBus::get().unsubscribe(this, &GameLogic::changePhase);

	delete phase;
}

void GameLogic::update(const float & dt)
{
	if (phase) {
		this->phase->update(dt);
	}
}

void GameLogic::changePhase(PhaseChangeEvent * event)
{
	Phase* previousPhase = phase;

	phase = event->newPhase;

	delete previousPhase;
}
