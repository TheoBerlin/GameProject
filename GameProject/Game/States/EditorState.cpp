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

	Level level;

	targetManager = new TargetManager();

	EntityManager* entityManager = &this->getEntityManager();

	Camera* c = new Camera(&camera, "Camera", { 0.0f, 0.5f, -10.0f });
	c->init();
	Display::get().getRenderer().setActiveCamera(c);

	level.entityManager = entityManager;
	level.targetManager = targetManager;
	level.collisionHandler = &this->collisionHandler;
	level.gui = &this->getGUI();

	levelParser.readLevel("./Game/Level/level.json", level);

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
	// Update entities.
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	for (unsigned int i = 0; i < entities.size(); i += 1) {
		entities[i]->update(dt);
	}
	entityWindow(entityManager);
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

void EditorState::entityWindow(EntityManager& entityManager)
{
#ifdef IMGUI

	ImGui::Begin("Entity Window");

	if (ImGui::BeginCombo("Entities", currentItem.c_str())) {
		for (int i = 0; i < entityManager.getEntitySize(); i++) {
			bool is_selected = (currentItem == entityManager.getEntity(i)->getName()); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(entityManager.getEntity(i)->getName().c_str(), is_selected)) {
				currentItem = entityManager.getEntity(i)->getName();
				currentEntity = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	if (currentEntity != -1) {
		glm::vec3 position = entityManager.getEntity(currentEntity)->getTransform()->getPosition();
		if(ImGui::InputFloat3("Position", &position[0], 2))
			entityManager.getEntity(currentEntity)->getTransform()->setPosition(position);
	}

	ImGui::End();
#endif
}
