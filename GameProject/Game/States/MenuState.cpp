#include "MenuState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"
#include "../../Engine/Entity/Entity.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/AssetManagement/ModelLoader.h"

MenuState::MenuState() : State()
{
	this->entity = new Entity();
	Camera* camera = new Camera("Camera");
	this->entity->addComponent(camera);
	camera->init();
	this->entity->setModel(ModelLoader::loadModel("cube.obj"));
	Display::get().getRenderer().setActiveCamera(camera);
}

MenuState::~MenuState()
{
	delete this->entity;
}

void MenuState::start()
{
}

void MenuState::end()
{
}

void MenuState::update(const float dt)
{
}

void MenuState::updateLogic()
{
}

void MenuState::render()
{
	Display::get().getRenderer().draw(this->entity);
}
