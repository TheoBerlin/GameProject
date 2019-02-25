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

#include "Engine/Config.h"

GameState::GameState()
{
	Level level;

	targetManager = new TargetManager();

	EntityManager* entityManager = &this->getEntityManager();
	level.entityManager = entityManager;
	level.targetManager = targetManager;
	level.collisionHandler = &this->collisionHandler;
	level.gui = &this->getGUI();

	levelParser.readLevel("./Game/Level/level.json", level);

	gameLogic.init(level);

	Display::get().getRenderer().initInstancing();

	InputHandler ih(Display::get().getWindowPtr());

	EventBus::get().subscribe(this, &GameState::emit);

	//Particle Emitter init
	ParticleManager::get().addEmitter(&emitter);
	emitter.setPosition(glm::vec3(0, 2.0f, -0.0f));
	emitter.setSpread(0.0f);
	emitter.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	emitter.setAcceleration(glm::vec3(0.0f, 0.0f, 0.0f));
	emitter.setMaxParticle(600);
	emitter.setSpawnRate(60);
	emitter.setStartColour(glm::vec4(0.8f, 0.0f, 1.0f, 1.0f));
	emitter.setEndColour(glm::vec4(0.8f, 0.0f, 1.0f, 0.0f));
	emitter.setLifeTime(10.0f);
	emitter.setScaleChange(1.0f);
	emitter.setScale(0.1f);
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

	//Particle tracing TEST
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
		Old rendering
	*/
	/*
	for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();
	*/

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

void GameState::emit(KeyEvent * evnt)
{
}
