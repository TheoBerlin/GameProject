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

	particleManger.addEmitter(&emitter);
	emitter.setPosition(glm::vec3(0, 2.0f, -0.0f));
	emitter.setSpread(0.0f);
	emitter.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
	emitter.setAcceleration(glm::vec3(0.0f, 0.0f, 0.0f));
	emitter.setMaxParticle(1000);
	emitter.setSpawnRate(100);
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

void GameState::update(const float dt)
{

	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	if (entityManager.getTracedEntity("ArrowReplay") != nullptr) {
		emitter.setPosition(entityManager.getTracedEntity("ArrowReplay")->getTransform()->getPosition());
		emitter.playEmitter(0);
	}
	for (Entity* entity : entities)
		entity->update(dt);
}

void GameState::updateLogic(const float dt)
{
	particleManger.update(1 / (float)FRAMES_PER_SECOND);
}

void GameState::render()
{
	//EntityManager& entityManager = this->getEntityManager();
	//std::vector<Entity*>& entities = entityManager.getAll();

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	renderer.pushParticleManager(&this->particleManger);

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
	if (evnt->key == GLFW_KEY_B)
		emitter.playEmitter(0);
}
