#include "TestState.h"

#include <Engine/Rendering/Display.h>
#include <Engine/States/StateManager.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Components/Camera.h>
#include <Engine/Components/ArrowGuider.h>
#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/InputHandler.h>

TestState::TestState() : State()
{
	InputHandler ih(Display::get().getWindowPtr());
	this->cameraEntity = new Entity({ 0.0f, 0.0f, -1.0f });
	cameraEntity->setModel(ModelLoader::loadModel("Game/assets/Arrow.fbx"));
	cameraEntity->getTransform()->setPosition(glm::vec3(0.0f, 0.0f, -2.0f));
	//new FreeMove(this->cameraEntity);

	Camera* camera = new Camera(this->cameraEntity, "Camera", { 0.0f, 1.0f, -2.0f });
	camera->init();
	Display::get().getRenderer().setActiveCamera(camera);

	ArrowGuider* arrowGuider = new ArrowGuider(cameraEntity);
	arrowGuider->startGuiding();

	this->entity = new Entity();
	this->entity->setModel(ModelLoader::loadModel("Game/assets/droneTarget.fbx"));
	this->entity->getTransform()->setPosition({ 0.0f, 0.0f, -2.0f });
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

void TestState::updateLogic(const float dt)
{
}

void TestState::render()
{
	Renderer& renderer = Display::get().getRenderer();
	renderer.push(this->entity);
	renderer.push(this->cameraEntity);
	renderer.drawAll();
}
