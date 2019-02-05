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
}

void TestState::updateLogic()
{
}

void TestState::render()
{
	Display::get().getRenderer().draw(this->entity);
	Display::get().getRenderer().draw(this->cameraEntity);
}
