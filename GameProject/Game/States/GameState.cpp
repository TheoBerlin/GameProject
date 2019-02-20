#include "GameState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Entity/EntityManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"

#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/InputHandler.h"

GameState::GameState() : gameLogic(&this->getEntityManager())
{
	EntityManager& entityManager = this->getEntityManager();
	levelParser.readEntites("./Engine/Level/level.json", &entityManager);

	Display::get().getRenderer().initInstancing();

	InputHandler ih(Display::get().getWindowPtr());

	EventBus::get().subscribe(this, &GameState::emit);

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

#include "Game/components/ArrowGuider.h"

void GameState::update(const float dt)
{

	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	if (entityManager.getTracedEntity("Player") != nullptr)
		dynamic_cast<ArrowGuider*>(entityManager.getTracedEntity("Player")->getComponent("ArrowGuider"))->setMovementSpeed(5.0f);
	if (entityManager.getTracedEntity("ArrowReplay") != nullptr) {
		emitter.setPosition(entityManager.getTracedEntity("ArrowReplay")->getTransform()->getPosition());
		emitter.playEmitter(0);
	}
	else {
		emitter.stopEmitter();
	}
	for (Entity* entity : entities)
		entity->update(dt);
	ParticleManager::get().update(dt);
}

void GameState::updateLogic(const float dt)
{

}

void GameState::render()
{
	//EntityManager& entityManager = this->getEntityManager();
	//std::vector<Entity*>& entities = entityManager.getAll();

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	//for (Entity* entity : entities)
	
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
}

void GameState::emit(KeyEvent * evnt)
{
}
