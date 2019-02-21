#include "GameState.h"

#include <Engine/States/StateManager.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>

#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Engine/InputHandler.h>

#include <Game/GameLogic/TargetManager.h>

#include "Engine/Config.h"

GameState::GameState()
{
	Level level;

	targetManager = new TargetManager();

	EntityManager* entityManager = &this->getEntityManager();
	level.entityManager = entityManager;
	level.targetManager = targetManager;
	level.collisionHandler = &this->collisionHandler;

	levelParser.readLevel("./Game/Level/level.json", level);

	gameLogic.init(level);

	Display::get().getRenderer().initInstancing();

	InputHandler ih(Display::get().getWindowPtr());
}

GameState::~GameState()
{
	delete targetManager;
}

void GameState::start()
{
	/*
		All entities in this state puts themselves in the rendering group of their model
	*/
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->attachToModel();
}

void GameState::end()
{
	/*
		All entities removes themselves from the rendering group of their model
	*/
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->detachFromModel();
}

void GameState::update(const float dt)
{
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->update(dt);
}

void GameState::updateLogic(const float dt)
{
	this->collisionHandler.checkCollision();
}

void GameState::render()
{
	//EntityManager& entityManager = this->getEntityManager();
	//std::vector<Entity*>& entities = entityManager.getAll();

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();
	
	/*
		Old rendering
	*/

	/*for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();*/


	/*
		New rendering
	*/
	renderer.drawAllInstanced();

#ifdef ENABLE_COLLISION_BOXES
	this->collisionHandler.updateDrawingData();
	this->collisionHandler.drawCollisionBoxes();
#endif
}
