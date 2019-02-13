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


	particleManger.addEmitter(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.25f, 4000, 1000, 0.1f, glm::vec3(0.0f, 0.0f, 0.0f));
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
	particleManger.update(1 / (float)FRAMES_PER_SECOND, glm::vec3(0.0f, -0.0f, 0.0f), 1.00f);
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
