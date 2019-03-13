#include "GameState.h"

#include <Engine/Collision/CollisionConfig.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include "Engine/Config.h"
#include <Engine/States/StateManager.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/GUIRenderer.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/GUI/GUI.h>
#include <Engine/InputHandler.h>
#include <Game/GameLogic/TargetManager.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/States/PauseState.h>

GameState::GameState(const std::string& levelJSON)
{
	Level level;

	targetManager = new TargetManager();

	EntityManager* entityManager = &this->getEntityManager();
	level.entityManager = entityManager;
	level.targetManager = targetManager;
	level.collisionHandler = &this->collisionHandler;
	level.gui = &this->getGUI();
	level.replaySystem = &this->replaySystem;
	level.scoreManager = &this->scoreManager;
	level.levelStructure = &this->levelStructure;
	level.lightManager = &this->lightManager;
	level.helpGUI = &this->helpGUI;

	this->helpGUI.init(&this->getGUI());

	levelParser.readLevel(levelJSON, level);

	Display::get().getRenderer().getPipeline()->addCurrentLightManager(level.lightManager);

	gameLogic.init(level);

	Display::get().getRenderer().initInstancing();
	Display::get().getRenderer().getPipeline()->setWallPoints(level.levelStructure->getWallPoints(), level.levelStructure->getWallGroupsIndex());

	InputHandler ih(Display::get().getWindowPtr());

	EventBus::get().subscribe(this, &GameState::exitGame);

	//For pause event
	this->hasSubscribedToPause = false;
}

GameState::~GameState()
{
	delete targetManager;

	Display::get().getRenderer().clearRenderingTargets();

	// Delete all loaded models
	ModelLoader::unloadAllModels();
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

	EventBus::get().unsubscribe(this, &GameState::pauseGame);
	EventBus::get().unsubscribe(this, &GameState::exitGame);
	this->hasSubscribedToPause = false;
}

void GameState::update(const float dt)
{
	// Update time for helpGUI for animations
	this->helpGUI.update(dt);

	if (!this->hasSubscribedToPause) {
		//Pause game event
		EventBus::get().subscribe(this, &GameState::pauseGame);
		this->hasSubscribedToPause = true;
	}

	// Update gamelogic
	this->gameLogic.update(dt);

	// Update entities.
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	ParticleManager::get().update(dt);

	for (unsigned int i = 0; i < entities.size(); i += 1) {
		entities[i]->update(dt);
	}

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	/*
		Update shaders
	*/
	renderer.updateShaders(dt);
}

void GameState::updateLogic(const float dt)
{
	this->collisionHandler.checkCollision();
}

void GameState::render()
{
	/*
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	*/
	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	/*
		New rendering
	*/
	renderer.drawAllInstanced();

#ifdef ENABLE_COLLISION_DEBUG_DRAW
	this->collisionHandler.updateDrawingData();
	this->collisionHandler.drawCollisionBoxes();
#endif

	// Draw gui elements.
	GUIRenderer& guiRenderer = display.getGUIRenderer();
	GUI& gui = this->getGUI();
	guiRenderer.draw(gui);
}

void GameState::pauseGame(PauseEvent* ev)
{
	this->pushState(new PauseState());
}

void GameState::exitGame(ExitEvent* ev)
{
	this->popState();
}
