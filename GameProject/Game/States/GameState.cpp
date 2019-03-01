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
#include "Game/components/ArrowGuider.h"

#include "Game/States/PauseState.h"

#include "Engine/Config.h"

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

	levelParser.readLevel(levelJSON, level);

	gameLogic.init(level);

	Display::get().getRenderer().initInstancing();

	InputHandler ih(Display::get().getWindowPtr());

	//For pause event
	this->hasSubscribedToPause = false;

	//Particle Emitter init
	ParticleManager::get().addEmitter(&emitter);
	emitter.setPosition(glm::vec3(0, 2.0f, -0.0f));
	emitter.setSpread(0.0f);
	emitter.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	emitter.setAcceleration(glm::vec3(0.0f, 0.0f, 0.0f));
	emitter.setMaxParticle(800);
	emitter.setSpawnRate(80);
	emitter.setStartColour(glm::vec4(0.8f, 0.0f, 1.0f, 1.0f));
	emitter.setEndColour(glm::vec4(0.8f, 0.0f, 1.0f, 0.0f));
	emitter.setLifeTime(10.0f);
	emitter.setScaleChange(1.0f);
	emitter.setScale(0.1f);
}

GameState::~GameState()
{
	delete targetManager;

	Display::get().getRenderer().clearRenderingModels();

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
	this->hasSubscribedToPause = false;
}

void GameState::update(const float dt)
{
	if (!this->hasSubscribedToPause) {

		//Pause game event
		EventBus::get().subscribe(this, &GameState::pauseGame);
		this->hasSubscribedToPause = true;
	}

	// Update entities.
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	//Particle tracing
	//Increase movement speed
	if (entityManager.getTracedEntity("PlayerArrow") != nullptr)
		if(entityManager.getTracedEntity("PlayerArrow")->getComponent("ArrowGuider") != nullptr)
			dynamic_cast<ArrowGuider*>(entityManager.getTracedEntity("PlayerArrow")->getComponent("ArrowGuider"))->setMovementSpeed(5.0f);
	//Get arrow replay position
	if (entityManager.getTracedEntity("ArrowReplay") != nullptr) {
		ParticleManager::get().update(dt);
		emitter.setPosition(entityManager.getTracedEntity("ArrowReplay")->getTransform()->getPosition());
		emitter.playEmitter(0);
	}
	else {
		emitter.stopEmitter();
	}

	for (unsigned int i = 0; i < entities.size(); i += 1) {
		entities[i]->update(dt);
	}


	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	/*
		Update shaders
	*/
	renderer.updateShaders(dt);

	this->replaySystem.update(dt);

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

#ifdef ENABLE_COLLISION_BOXES
	this->collisionHandler.updateDrawingData();
	this->collisionHandler.drawCollisionBoxes();
#endif

	// Draw gui elements.
	GUIRenderer& guiRenderer = display.getGUIRenderer();
	GUI& gui = this->getGUI();
	guiRenderer.draw(gui);
}

void GameState::pauseGame(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_ESCAPE && ev->action == GLFW_PRESS) {

		this->pushState(new PauseState());
	}
}
