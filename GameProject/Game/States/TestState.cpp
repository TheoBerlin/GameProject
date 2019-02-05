#include "TestState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"
#include "../../Engine/Entity/Entity.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/AssetManagement/ModelLoader.h"
#include "../../Engine/InputHandler.h"

TestState::TestState() : State()
{
	InputHandler ih(Display::get().getWindowPtr());
	this->cameraEntity = new Entity({ 0.0f, 0.0f, -1.0f });
	this->cameraEntity->addComponent(new FreeMove());
	Camera* camera = new Camera("Camera", { 0.0f, 1.0f, -2.0f });
	this->cameraEntity->addComponent(camera);
	camera->init();
	Display::get().getRenderer().setActiveCamera(camera);

	this->entity = new Entity();
	this->entity->setModel(ModelLoader::loadModel("Game/assets/cube.obj"));
	this->entity->getMatrix()->setPosition({ 0.0f, 0.0f, -2.0f });

	EventBus::get().subscribe(this, &TestState::moveCube);
}

TestState::~TestState()
{
	delete this->cameraEntity;
	delete this->entity;
}

void TestState::start()
{
}

void TestState::end()
{
}

void TestState::update(const float dt)
{
	this->cameraEntity->update(dt);
	this->entity->update(dt);
	//entity->getMatrix()->rotate(glm::vec3(5 * dt, 0, 0), glm::vec3(0, 0, -1));
	//entity->getMatrix()->translate(glm::vec3(10 *dt, 0, 0));
	entity->getMatrix()->rotate(glm::vec3(0, (3.1415 / 2) * dt, 0));
	entity->getMatrix()->scale(1.1 *dt);
	LOG_PRINT("%f, %f, %f", entity->getMatrix()->getPosition().x, entity->getMatrix()->getPosition().y, entity->getMatrix()->getPosition().z);
}

void TestState::updateLogic()
{
}

void TestState::render()
{
	Renderer& renderer = Display::get().getRenderer();
	renderer.push(this->entity);
	renderer.push(this->cameraEntity);
	renderer.drawAll();
}

void TestState::moveCube(KeyEvent * e)
{
	if (e->key == GLFW_KEY_0)
		entity->getMatrix()->setScale(glm::vec3(5, 5, 0.1));

}
