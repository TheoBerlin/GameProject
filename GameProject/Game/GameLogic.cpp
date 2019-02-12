#include "GameLogic.h"


#include "../Engine/Rendering/Display.h"
#include "../Engine/Rendering/Renderer.h"
#include "../Engine/Components/Component.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/Components/ArrowGuider.h"
#include "../Engine/AssetManagement/ModelLoader.h"
#include "Engine/Events/EventBus.h"

GameLogic::GameLogic(EntityManager * em)
{
	this->em = em;
	/*
		Start game in phase 1
	*/
	this->currentPhase = Phases::PHASE_ONE;
	this->enterPhaseOne(glm::vec3(-10.0f, 20.0f, 10.0f), glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)));

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
	/*
		Leave old phase handle entity removal
	*/
	switch (this->currentPhase) {
	case Phases::PHASE_ONE:
		this->leavePhaseOne();
		break;
	case Phases::PHASE_TWO:
		this->leavePhaseTwo();
		break;
	case Phases::PHASE_THREE:

		break;
	}

	/*
		Enter new phase handle entity adding
	*/
	switch (phase) {
	case Phases::PHASE_ONE:
		this->enterPhaseOne(glm::vec3(-10.0f, 20.0f, 10.0f), glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)));
		break;
	case Phases::PHASE_TWO:
		this->enterPhaseTwo(glm::vec3(1.0f, 1.0f, 2.0f));
		break;
	case Phases::PHASE_THREE:

		break;
	}

}

void GameLogic::enterPhaseOne(const glm::vec3 & cameraPos, const glm::vec3 & cameraDir)
{
	this->currentPhase = Phases::PHASE_ONE;
	/*
		Create camera entity
	*/
	this->camera = this->em->addTracedEntity("PhaseOneCamera");
	this->camera->getTransform()->setPosition(cameraPos);
	this->camera->getTransform()->setForward(cameraDir);

	Camera* camera = new Camera(this->camera, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	Display::get().getRenderer().setActiveCamera(camera);

}

void GameLogic::enterPhaseTwo(const glm::vec3 & playerPos)
{
	this->currentPhase = Phases::PHASE_TWO;
	/*
		Create arrow entity
	*/
	Entity * entity = this->em->addTracedEntity("Player");
	entity->getTransform()->setPosition(playerPos);
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 0.5f));
	entity->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

	/*
		Add camera to arrow entity
	*/
	Camera* camera = new Camera(entity, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	/*
		Add arrowguider to entity
	*/
	ArrowGuider* arrow = new ArrowGuider(entity, 10.0f);
	arrow->startGuiding();

	Display::get().getRenderer().setActiveCamera(camera);
}

void GameLogic::leavePhaseOne()
{
	this->em->removeTracedEntity("PhaseOneCamera");
}

void GameLogic::leavePhaseTwo()
{
	this->em->removeTracedEntity("Player");

}

void GameLogic::changePhaseCallback(KeyEvent * ev)
{
	// Toggle keys
	if (ev->key == GLFW_KEY_1 && ev->action == GLFW_PRESS && this->currentPhase != Phases::PHASE_ONE)
	{
		this->changePhase(Phases::PHASE_ONE);
	}
	else if (ev->key == GLFW_KEY_2 && ev->action == GLFW_PRESS && this->currentPhase != Phases::PHASE_TWO) 
	{
		this->changePhase(Phases::PHASE_TWO);
	}
	//else if (ev->key == GLFW_KEY_3 && ev->action == GLFW_PRESS && this->currentPhase != Phases::PHASE_THREE) 
	//{
	//	this->changePhase(Phases::PHASE_THREE);
	//}

}
