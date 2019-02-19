#include "GameState.h"

#include <Engine/States/StateManager.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/GUI/GUI.h>
#include <Engine/Rendering/GUIRenderer.h>

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
	level.gui = &this->getGUI();

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
	// Update entities.
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->update(dt);
}

void GameState::updateLogic(const float dt)
{
	
}

void GameState::render()
{
	//EntityManager& entityManager = this->getEntityManager();
	//std::vector<Entity*>& entities = entityManager.getAll();

	// Draw entities.
	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();
	
	/*
		Old rendering
	*/

	/*for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();
	*/

	/*
		New rendering
	*/
	renderer.drawAllInstanced();

	// Draw gui elements.
	GUIRenderer& guiRenderer = display.getGUIRenderer();
	GUI& gui = this->getGUI();
	guiRenderer.draw(gui);
}
