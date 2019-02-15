#include "GameLogic.h"


#include "../Engine/Rendering/Display.h"
#include "../Engine/Rendering/Renderer.h"
#include "../Engine/Components/Component.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/Components/ArrowGuider.h"
#include "../Engine/Components/Target.h"
#include "../Engine/Components/PlayerCollision.h"
#include "../Engine/AssetManagement/ModelLoader.h"
#include "Engine/Events/EventBus.h"
#include "Engine/Collision/CollisionHandler.h"

GameLogic::GameLogic(EntityManager * em, CollisionHandler * ch)
{
	this->em = em;
	this->ch = ch;
	/*
		Start game in phase 1
	*/
	this->currentPhase = Phases::PHASE_ONE;
	this->enterPhaseOne(glm::vec3(-10.0f, 20.0f, 10.0f), glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)));

	/*
		Change between phases using key 1 and 2.
	*/
	EventBus::get().subscribe(this, &GameLogic::changePhaseCallback);

	/*
		Do stuff when collision happens
	*/
	EventBus::get().subscribe(this, &GameLogic::playerCollisionCallback);
}


GameLogic::~GameLogic()
{
	EventBus::get().unsubscribe(this, &GameLogic::changePhaseCallback);
	EventBus::get().unsubscribe(this, &GameLogic::playerCollisionCallback);
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
		this->enterPhaseTwo(glm::vec3(1.0f, 1.0f, 12.0f));
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

	Camera* camera = new Camera(this->camera, "Camera", { 0.0f, 0.5f, -2.0f });
	camera->init();

	Display::get().getRenderer().setActiveCamera(camera);

}

void GameLogic::enterPhaseTwo(const glm::vec3 & playerPos)
{
	this->currentPhase = Phases::PHASE_TWO;
	/*
		Create arrow entity
	*/
	this->player = this->em->addTracedEntity("Player");
	this->player->getTransform()->setPosition(playerPos);
	this->player->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));
	this->player->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));
	ch->addCollisionToEntity(this->player, SHAPE::ARROW);

	/*
		Add camera to arrow entity
	*/
	Camera* camera = new Camera(this->player, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	/*
		Add arrowguider to entity
	*/
	ArrowGuider* arrow = new ArrowGuider(this->player, 2.0f);
	arrow->startGuiding();

	/*
		Add player to entity
	*/
	new PlayerCollision(this->player);

	Display::get().getRenderer().setActiveCamera(camera);
}

void GameLogic::leavePhaseOne()
{
	this->em->removeTracedEntity("PhaseOneCamera");
}

void GameLogic::leavePhaseTwo()
{
	this->ch->removeCollisionBody(this->em->getTracedEntity("Player"));
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

void GameLogic::playerCollisionCallback(PlayerCollisionEvent * ev)
{
	// Entity1 should always be player, but to be on the safe side...
	Entity* otherEntity;
	const rp3d::ProxyShape* playerShape;
	const rp3d::ProxyShape* otherShape;
	if (ev->entity1 != this->player)
	{
		otherEntity = ev->entity1;
		otherShape = ev->shape1;
		playerShape = ev->shape2;
	}
	else
	{
		otherEntity = ev->entity2;
		otherShape = ev->shape2;
		playerShape = ev->shape1;
	}

	// Handle collision for the entity the arrow hit
	CollisionComponent* collision = dynamic_cast<CollisionComponent*>(otherEntity->getComponent("Collision"));
	if (collision != nullptr)
		collision->handleCollision(otherShape, playerShape);

	// Handle collision for the player arrow
	CollisionComponent* playerCollision = dynamic_cast<CollisionComponent*>(this->player->getComponent("Collision"));
	if (playerCollision != nullptr)
		playerCollision->handleCollision(playerShape, otherShape);
}
