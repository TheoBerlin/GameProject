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

	/*Entity* cameraEntity = entityManager.addTracedEntity("Camera");
	new FreeMove(cameraEntity);
	Camera* camera = new Camera(cameraEntity, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();
	cameraEntity->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

	Display::get().getRenderer().setActiveCamera(camera);*/

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
