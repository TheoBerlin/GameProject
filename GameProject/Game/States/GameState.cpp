#include "GameState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Entity/EntityManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"

#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/InputHandler.h"

GameState::GameState()
{
	EntityManager& entityManager = this->getEntityManager();
	levelParser.readEntites("./Engine/Level/level.json", &entityManager);

	this->collisionHandler = new CollisionHandler();
	this->collisionHandler->createCollisionBodies(2);
	this->collisionHandler->addCollisionToEntity(entityManager.getTracedEntity("Camera"), SHAPE::BOX);
	this->collisionHandler->addCollisionToEntity(entityManager.getTracedEntity("Target1"), SHAPE::BOX);
	//this->collisionHandler->removeCollisionBody(entityManager.getTracedEntity("Target1"));

	InputHandler ih(Display::get().getWindowPtr());
}

GameState::~GameState()
{
	delete this->collisionHandler;
}

void GameState::start()
{
}

void GameState::end()
{
}

void GameState::update(const float dt)
{
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->update(dt);
}

void GameState::updateLogic()
{
	this->collisionHandler->update(1);
}

void GameState::render()
{
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();
	for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();
}
