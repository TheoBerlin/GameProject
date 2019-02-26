#include "EditorState.h"

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
#include "Engine/Imgui/imgui.h"

#include "Engine/Config.h"

EditorState::EditorState()
{
	targetManager = new TargetManager();

	EntityManager* entityManager = &this->getEntityManager();

	// Set up the player camera
	Transform* camTransform = camera.getTransform();

	camTransform->setPosition(glm::vec3(0.0f, 5.0, -2.0f));
	camTransform->setForward(glm::vec3(0.0f, 0.0f, 1.0f));
	camTransform->resetRoll();

	Camera* c = new Camera(&camera, "Camera");
	c->init();

	freeMove = new FreeMove(&camera);

	Display::get().getRenderer().setActiveCamera(c);

	level.entityManager = entityManager;
	level.targetManager = targetManager;
	level.collisionHandler = &this->collisionHandler;
	level.gui = &this->getGUI();

	Display::get().getRenderer().initInstancing();

	InputHandler ih(Display::get().getWindowPtr());

}

EditorState::~EditorState()
{
	delete targetManager;
}

void EditorState::start()
{
	/*
	All entities in this state puts themselves in the rendering group of their model
	*/
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->attachToModel();
}

void EditorState::end()
{
	/*
	All entities removes themselves from the rendering group of their model
*/
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->detachFromModel();
}

void EditorState::update(const float dt)
{
	freeMove->update(dt);
	camera.update(dt);

	// Update entities.
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	for (unsigned int i = 0; i < entities.size(); i += 1) {
		entities[i]->update(dt);
	}

	mainWindow(entityManager);
}

void EditorState::updateLogic(const float dt)
{
	this->collisionHandler.checkCollision();
}

void EditorState::render()
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

void EditorState::mainWindow(EntityManager & entityManager)
{
#ifdef IMGUI
	ImGui::Begin("Main Window");
	ImGui::SetWindowPos({0, 0});

	if (ImGui::Button("Level"))
		activeWindow[0] = !activeWindow[0];
	if (ImGui::Button("Entities"))
		activeWindow[1] = !activeWindow[1];
	ImGui::NewLine();

	ImGui::End();
#endif

	if (activeWindow[0])
		levelWindow();
	if (activeWindow[1])
		entityWindow(entityManager);
}

void EditorState::entityWindow(EntityManager& entityManager)
{
#ifdef IMGUI
	ImGui::Begin("Entity Window");
	ImGui::AlignTextToFramePadding();
	if (ImGui::BeginCombo("Entities", currentItem.c_str())) {
		for (int i = 0; i < entityManager.getEntitySize(); i++) {
			bool is_selected = (currentItem == entityManager.getEntity(i)->getName()); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(entityManager.getEntity(i)->getName().c_str(), is_selected)) {
				currentItem = entityManager.getEntity(i)->getName();
				currentModel = entityManager.getEntity(i)->getModel()->getName();
				currentEntity = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	if (currentEntity != -1) {
		//Entity Info
		std::string name = entityManager.getEntity(currentEntity)->getName();
		glm::vec3 position = entityManager.getEntity(currentEntity)->getTransform()->getPosition();
		glm::vec3 scale = entityManager.getEntity(currentEntity)->getTransform()->getScale();
		ImGui::Text("Entity Info");
		if (ImGui::InputText("Name", &name[0], 64))
			entityManager.getEntity(currentEntity)->setName(name.c_str());
		if(ImGui::InputFloat3("Position", &position[0], 2))
			entityManager.getEntity(currentEntity)->getTransform()->setPosition(position);
		if (ImGui::InputFloat3("Scale", &scale[0], 2))
			entityManager.getEntity(currentEntity)->getTransform()->setScale(scale);
		//Model Info
		Model* model = entityManager.getEntity(currentEntity)->getModel();
		ImGui::Text("Model Info");
		ImGui::InputText("Model Name", &currentModel[0], 64);
		if (ImGui::Button("Load Model")) {
			model = ModelLoader::loadModel(std::string("./Game/assets/") + currentModel.c_str() + ".fbx");
			entityManager.getEntity(currentEntity)->setModel(model);
			entityManager.getEntity(currentEntity)->getModel()->setName(currentModel);
		}
		ImGui::NewLine();

	}
	ImGui::End();
#endif
}
#include <string>
#include <string.h>
#include <iostream>

void EditorState::levelWindow()
{
#ifdef IMGUI
	ImGui::Begin("Level Window");
	levelName.reserve(64);
	ImGui::InputText("LevelName", (char*)levelName.c_str(), levelName.capacity());
	if (ImGui::Button("Load")) {
		levelParser.readLevel(std::string("./Game/Level/") + levelName.c_str() + ".json", level);
	}
	ImGui::End();
#endif
}
