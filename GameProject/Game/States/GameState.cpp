#include "GameState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Entity/EntityManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"

#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/InputHandler.h"

GameState::GameState()
{
	EntityManager& entityManager = this->getEntityManager();
	levelParser.readEntites("./Engine/Level/level.json", &entityManager);

	InputHandler ih(Display::get().getWindowPtr());

	EventBus::get().subscribe(this, &GameState::emit);

	particleManger.addEmitter(&emitter);
	particleManger.addEmitter(&emitter1);
	particleManger.addEmitter(&emitter2);
	particleManger.addEmitter(&emitter3);
	emitter.setPosition(glm::vec3(0, 2.0f, -0.0f));
	emitter.setSpread(2.0f);
	emitter.setVelocity(glm::vec3(0.0f, 2.0f, 0.0f));
	emitter.setAcceleration(glm::vec3(0.0f, 0.0f, 1.0f));
	emitter.setMaxParticle(2000);
	emitter.setSpawnRate(2000);
	emitter.setStartColour(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	emitter.setEndColour(glm::vec4(1.0f, 1.0f, 0.8f, 1.0f));
	emitter.setLifeTime(1.0f);
	emitter.setScaleChange(0.8f);
	emitter.setScale(0.2f);
	emitter1 = emitter;
	emitter1.setVelocity(glm::vec3(0.0, 1.0f, 0.0f));
	emitter1.setStartColour(glm::vec4(1.0f));
	emitter1.playEmitter(0);
	emitter2 = emitter1;
	emitter2.setVelocity(glm::vec3(0.0, 10.0f, 0.0f));
	emitter2.setStartColour(glm::vec4(1.0f));
	emitter2.playEmitter(0);
	emitter3 = emitter1;
	emitter3.setVelocity(glm::vec3(0.0, 10.0f, 0.0f));
	emitter1.setPosition(glm::vec3(0.0, 10.0f, 0.0f));
	emitter3.setStartColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	emitter3.playEmitter(0);
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
	particleManger.update(1 / (float)FRAMES_PER_SECOND);
}

void GameState::render()
{
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	renderer.pushParticleManager(&this->particleManger);

	for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();
}

void GameState::emit(KeyEvent * evnt)
{
	if (evnt->key == GLFW_KEY_B)
		emitter.playEmitter(0);
}
