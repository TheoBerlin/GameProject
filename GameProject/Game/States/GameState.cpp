#include "GameState.h"

#include <Engine/States/StateManager.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>

#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Engine/InputHandler.h>

#include <Game/GameLogic/TargetManager.h>

GameState::GameState()
{
	Level level;

	targetManager = new TargetManager();

	level.entityManager = &this->getEntityManager();
	level.targetManager = targetManager;

	levelParser.readLevel("./Game/Level/level.json", level);

	gameLogic.init(level);

	InputHandler ih(Display::get().getWindowPtr());
}

GameState::~GameState()
{
	delete targetManager;
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
