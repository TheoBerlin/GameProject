#include "GameLogic.h"

#include "../Engine/Rendering/Display.h"
#include "../Engine/Rendering/Renderer.h"
#include "../Engine/Components/Component.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/AssetManagement/ModelLoader.h"
#include "Engine/Events/EventBus.h"
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathTreader.h>

GameLogic::GameLogic()
{
}

void GameLogic::init(Level& level)
{
	this->em = level.entityManager;
	this->targetManager = level.targetManager;
	/*
		Start game in phase 1
	*/
	this->currentPhase = Phases::PHASE_OVERVIEW;
	this->enterOverviewPhase(glm::vec3(-10.0f, 20.0f, 10.0f), glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)));

	/*
		Change between phases using key 1 and 2.
	*/
	EventBus::get().subscribe(this, &GameLogic::changePhaseCallback);
}

GameLogic::~GameLogic()
{
	EventBus::get().unsubscribe(this, &GameLogic::changePhaseCallback);
}

void GameLogic::changePhase(Phases phase)
{
	Phases previousPhase = this->currentPhase;
	/*
		Enter new phase handle entity adding
	*/
	switch (phase) {
	case Phases::PHASE_OVERVIEW:
		this->enterOverviewPhase(glm::vec3(-10.0f, 20.0f, 10.0f), glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)));
		break;
	case Phases::PHASE_GUIDING:
		this->enterGuidingPhase(glm::vec3(1.0f, 1.0f, 12.0f));
		break;
	case Phases::PHASE_REPLAY:
		this->enterReplayPhase(glm::vec3(1.0f, 1.0f, 16.0f));
		break;
	}

	/*
		Leave old phase handle entity removal
	*/
	switch (previousPhase) {
	case Phases::PHASE_OVERVIEW:
		this->leaveOverviewPhase();
		break;
	case Phases::PHASE_GUIDING:
		this->leaveGuidingPhase();
		break;
	case Phases::PHASE_REPLAY:
		this->leaveReplayPhase();
		break;
	}
}

void GameLogic::enterOverviewPhase(const glm::vec3 & cameraPos, const glm::vec3 & cameraDir)
{
	this->currentPhase = Phases::PHASE_OVERVIEW;
	/*
		Create camera entity
	*/
	this->camera = this->em->addTracedEntity("PhaseOneCamera");
	this->camera->getTransform()->setPosition(cameraPos);
	this->camera->getTransform()->setForward(cameraDir);

	Camera* camera = new Camera(this->camera, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

	// Reset targets
	targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);

}

void GameLogic::enterGuidingPhase(const glm::vec3 & playerPos)
{
	this->currentPhase = Phases::PHASE_GUIDING;
	/*
		Create arrow entity
	*/
	Entity * entity = this->em->addTracedEntity("Player");
	entity->getTransform()->setPosition(playerPos);
	entity->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));
	entity->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

	/*
		Add camera to arrow entity
	*/
	Camera* camera = new Camera(entity, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	/*
		Add arrowguider to entity
	*/
	ArrowGuider* arrow = new ArrowGuider(entity, 2.0f);
	arrow->startGuiding();

	// Reset targets
	targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

void GameLogic::enterReplayPhase(const glm::vec3 & arrowPos)
{
	this->currentPhase = Phases::PHASE_REPLAY;

	// Create free camera
	Entity* playerEntity = this->em->addTracedEntity("PlayerCamera");
	playerEntity->getTransform()->setPosition(glm::vec3(0.0f, 3.0f, 4.0f));
	playerEntity->getTransform()->setForward(glm::vec3(0.0f, -0.7f, -0.7f));
	Camera* camera = new Camera(playerEntity, "Camera");
	camera->init();

	FreeMove* freeMove = new FreeMove(playerEntity);

	/*
		Create arrow entity
	*/
	Entity * arrowEntity = this->em->addTracedEntity("ArrowReplay");
	arrowEntity->getTransform()->setPosition(arrowPos);
	arrowEntity->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));
	arrowEntity->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

	// Copy path
	Entity* oldPlayerEntity = this->em->getTracedEntity("Player");

	if (oldPlayerEntity) {
		Component* tmpPtr = oldPlayerEntity->getComponent("ArrowGuider");
		ArrowGuider* oldArrowGuider = dynamic_cast<ArrowGuider*>(tmpPtr);

		oldArrowGuider->stopGuiding();

		// Add path treader to entity
		PathTreader* arrow = new PathTreader(arrowEntity, oldArrowGuider->getPath());
		arrow->startTreading();

		// Add path visualizer for debugging
		PathVisualizer* pathVisualizer = new PathVisualizer(arrowEntity, this->em);
		pathVisualizer->addPath(oldArrowGuider->getPath());
	}

	// Reset targets
	targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

void GameLogic::leaveOverviewPhase()
{
	this->em->removeTracedEntity("PhaseOneCamera");
}

void GameLogic::leaveGuidingPhase()
{
	this->em->removeTracedEntity("Player");
}

void GameLogic::leaveReplayPhase()
{
	Entity* oldPlayerEntity = this->em->getTracedEntity("ArrowReplay");
	Component* tmpPtr = oldPlayerEntity->getComponent("PathVisualizer");

	if (tmpPtr) {
		PathVisualizer* pathVisualizer = dynamic_cast<PathVisualizer*>(tmpPtr);

		pathVisualizer->removeVisualizers();

	}

	this->em->removeTracedEntity("ArrowReplay");
	this->em->removeTracedEntity("PlayerCamera");
}

void GameLogic::changePhaseCallback(KeyEvent * ev)
{
	// Toggle keys
	if (ev->key == GLFW_KEY_1 && ev->action == GLFW_PRESS && this->currentPhase != Phases::PHASE_OVERVIEW)
	{
		this->changePhase(Phases::PHASE_OVERVIEW);
	}
	else if (ev->key == GLFW_KEY_2 && ev->action == GLFW_PRESS && this->currentPhase != Phases::PHASE_GUIDING) 
	{
		this->changePhase(Phases::PHASE_GUIDING);
	}
	else if (ev->key == GLFW_KEY_3 && ev->action == GLFW_PRESS && this->currentPhase != Phases::PHASE_REPLAY) 
	{
		this->changePhase(Phases::PHASE_REPLAY);
	}
}
