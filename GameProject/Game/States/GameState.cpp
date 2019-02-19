#include "GameState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Entity/EntityManager.h"
#include "../../Engine/GUI/GUI.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"
#include "../../Engine/Rendering/GUIRenderer.h"

#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/InputHandler.h"

GameState::GameState() : gameLogic(&this->getEntityManager())
{
	EntityManager& entityManager = this->getEntityManager();
	levelParser.readEntites("./Engine/Level/level.json", &entityManager);

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
	// Update entities.
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

	// Draw entities.
	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();
	for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();

	// Draw gui elements.
	GUIRenderer& guiRenderer = display.getGUIRenderer();
	GUI& gui = this->getGUI();
	guiRenderer.draw(gui);
}
