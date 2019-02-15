#include "GameState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Entity/EntityManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"

#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/InputHandler.h"

// ---- TEMP ----
#include "../../Engine/Components/Target.h"

GameState::GameState() : gameLogic(&this->getEntityManager(), &this->collisionHandler)
{
	EntityManager& entityManager = this->getEntityManager();
	levelParser.readEntites("./Engine/Level/level.json", &entityManager);

	this->collisionHandler.createCollisionBodies(4);
	new Target(entityManager.getTracedEntity("Target1"));
	new Target(entityManager.getTracedEntity("Target2"));
	new Target(entityManager.getTracedEntity("Target3"));
	this->collisionHandler.addCollisionToEntity(entityManager.getTracedEntity("Target1"), SHAPE::BOX);
	this->collisionHandler.addCollisionToEntity(entityManager.getTracedEntity("Target2"), SHAPE::BOX);
	this->collisionHandler.addCollisionToEntity(entityManager.getTracedEntity("Target3"), SHAPE::BOX);

	InputHandler ih(Display::get().getWindowPtr());
}

GameState::~GameState()
{
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
	this->collisionHandler.update(1);
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
