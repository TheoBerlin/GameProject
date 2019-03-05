#include "EditorState.h"

#include <Engine/States/StateManager.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/GUI/GUI.h>
#include <Engine/Rendering/GUIRenderer.h>

#include <Engine/Components/FreeMove.h>
#include <Engine/Components/Camera.h>
#include <Game/components/Hover.h>
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

	camTransform->setPosition(glm::vec3(0.0f, 1.0, 4.0f));
	camTransform->setForward(glm::vec3(0.0f, 0.0f, -1.0f));
	camTransform->resetRoll();

	Camera* c = new Camera(&camera, "Camera");
	c->init();

	freeMove = new FreeMove(&camera);
	camSpeed = freeMove->getSpeed();

	Display::get().getRenderer().setActiveCamera(c);

	level.entityManager = entityManager;
	level.targetManager = targetManager;
	level.scoreManager = &this->scoreManager;
	level.collisionHandler = &this->collisionHandler;
	level.gui = &this->getGUI();

	EventBus::get().subscribe(this, &EditorState::pauseGame);
	InputHandler ih(Display::get().getWindowPtr());

	ImGui::GetIO().KeyMap[ImGuiKey_Backspace] = ImGuiKey_Backspace;
}

EditorState::~EditorState()
{
	delete targetManager;
	EventBus::get().unsubscribe(this, &EditorState::pauseGame);

	Display::get().getRenderer().clearRenderingModels();

	// Delete all loaded models
	ModelLoader::unloadAllModels();
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

	for (unsigned int i = 0; i < entities.size(); i++) {
		entities[i]->update(dt);
	}

	mainWindow(entityManager);
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
	ImGui::SetWindowSize({ 110, 150 });

	if (ImGui::Button("Level"))
		activeWindow[0] = !activeWindow[0];
	if (ImGui::Button("Entities"))
		activeWindow[1] = !activeWindow[1];
	if (ImGui::Button("Player"))
		activeWindow[2] = !activeWindow[2];
	if (ImGui::Button("Camera"))
		activeWindow[3] = !activeWindow[3];
	ImGui::NewLine();

	ImGui::End();
#endif

	if (activeWindow[0])
		levelWindow(entityManager);
	if (activeWindow[1])
		entityWindow(entityManager);
	if (activeWindow[2])
		playerWindow(entityManager);
	if (activeWindow[3])
		editorWindow();
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
				currentIsTarget = false;
				for (int j = 0; j < targetManager->getMovingTargets().size(); j++) {
					if (entityManager.getEntity(i) == targetManager->getMovingTargets()[j].pathTreader->getHost()) {
						currentIsTarget = true;
						break;
					}
				}
				for (int j = 0; j < targetManager->getStaticTargets().size(); j++) {
					if (entityManager.getEntity(i) == targetManager->getStaticTargets()[j].hoverAnimation->getHost()) {
						currentIsTarget = true;
						break;
					}
				}
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	if (ImGui::Button("Add Entity")) {
		Entity* newEntity = new Entity();
		newEntity->setName("Entity" + std::to_string(entityManager.getEntitySize()));
		Model* model = ModelLoader::loadModel("./Game/assets/Cube.fbx");
		newEntity->setModel(model);
		newEntity->getModel()->setName("Cube");
		entityManager.addEntity(newEntity);
		currentModel = newEntity->getModel()->getName();
		currentEntity = entityManager.getEntitySize() - 1;
		currentIsTarget = false;
	}
	if (ImGui::Button("Remove Entity")) {
		if (currentIsTarget) {
			for (int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
				if (level.targetManager->getMovingTargets()[i].pathTreader->getName() == entityManager.getEntity(currentEntity)->getName()) {
					level.targetManager->removeTarget(entityManager.getEntity(currentEntity)->getName());
				}
			}
			for (int i = 0; i < level.targetManager->getStaticTargets().size(); i++) {
				if (level.targetManager->getStaticTargets()[i].hoverAnimation->getName() == entityManager.getEntity(currentEntity)->getName()) {
					level.targetManager->removeTarget(entityManager.getEntity(currentEntity)->getName());
				}
			}
		}
		entityManager.removedEntity(currentEntity);
		currentEntity = -1;
	}
	if (currentEntity != -1) {
		Entity* curEntity = entityManager.getEntity(currentEntity);
		//Entity Info
		std::string name = curEntity->getName();
		glm::vec3 position = curEntity->getTransform()->getPosition();
		glm::vec3 scale = curEntity->getTransform()->getScale();
		glm::vec3 rotation = curEntity->getTransform()->getRotation();

		ImGui::Text("Entity Info");
		if (ImGui::InputText("Name", &name[0], 64))
			curEntity->setName(name.c_str());
		if(ImGui::InputFloat3("Position", &position[0], 2))
			curEntity->getTransform()->setPosition(position);
		if (ImGui::InputFloat3("Scale", &scale[0], 2))
			curEntity->getTransform()->setScale(scale);
		if (ImGui::InputFloat3("Rotation", &rotation[0], 2))
			curEntity->getTransform()->setRotation(rotation);
		if (ImGui::RadioButton("IsTarget", currentIsTarget)) {
			currentIsTarget = !currentIsTarget;
		}
		if (currentIsTarget) {
			ImGui::SameLine();
			if (ImGui::Button("Add Path")) {
				bool found = false;
				for (int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
					if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
						level.targetManager->getMovingTargets()[i].pathTreader->getPath().push_back(KeyPoint(glm::vec3(0.0f), 1.0f));
						found = true;
					}
				}
				if (!found) {
					for (int i = 0; i < level.targetManager->getStaticTargets().size(); i++) {
						if (level.targetManager->getStaticTargets()[i].hoverAnimation->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
							std::vector<KeyPoint> path;
							path.push_back(KeyPoint(glm::vec3(0.0f), 1.0f));
							path.push_back(KeyPoint(glm::vec3(1.0f), 1.0f));
							level.targetManager->removeTarget(entityManager.getEntity(currentEntity)->getName());
							level.targetManager->addMovingTarget(entityManager.getEntity(currentEntity), path);
						}
					}
				}
			}

			if (level.targetManager->getMovingTargets().size() > 0) {
				ImGui::SameLine();
				if (ImGui::Button("Remove Path")) {
					for (int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
						if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
							level.targetManager->getMovingTargets()[i].pathTreader->getPath().pop_back();
							if (level.targetManager->getMovingTargets()[i].pathTreader->getPath().size() == 0) {
								level.targetManager->addStaticTarget(level.targetManager->getMovingTargets()[i].pathTreader->getHost());
								level.targetManager->removeTarget(level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName());
							}
						}
					}
				}
			}
			for (int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
				if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
					for (int j = 0; j < level.targetManager->getMovingTargets().size(); j++) {
						ImGui::InputFloat3(std::string("Path " + std::to_string(j)).c_str(), &level.targetManager->getMovingTargets()[i].pathTreader->getPath()[j].Position[0], 2);
						ImGui::InputFloat(std::string("Path Time " + std::to_string(j)).c_str(), &level.targetManager->getMovingTargets()[i].pathTreader->getPath()[j].t);
					}
				}
			}
		}
		//Model Info
		Model* model = curEntity->getModel();
		ImGui::Text("Model Info");
		ImGui::InputText("Model Name", &currentModel[0], 64);
		if (ImGui::Button("Load Model")) {
			model = ModelLoader::loadModel(std::string("./Game/assets/") + currentModel.c_str() + ".fbx");
			curEntity->setModel(model);
			curEntity->getModel()->setName(currentModel);
		}
		ImGui::NewLine();

	}
	ImGui::End();
#endif
}

void EditorState::levelWindow(EntityManager& entityManager)
{
#ifdef IMGUI
	ImGui::Begin("Level Window");
	levelName.reserve(64);
	ImGui::InputText("LevelName", (char*)levelName.c_str(), levelName.capacity());
	if (ImGui::Button("Save")) {
		levelParser.writeLevel(std::string("./Game/Level/") + levelName.c_str() + ".json", level);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		entityManager.removeEntities();

		levelParser.readLevel(std::string("./Game/Level/") + levelName.c_str() + ".json", level);
		Display::get().getRenderer().initInstancing();
	}
	ImGui::End();
#endif
}

void EditorState::playerWindow(EntityManager & entityManager)
{
#ifdef IMGUI
	ImGui::Begin("Player Window");

	ImGui::InputFloat3("Overview Position", &level.player.oversightCamera.position[0], 2);
	ImGui::InputFloat3("Overview Direction", &level.player.oversightCamera.direction[0], 2);
	ImGui::InputFloat3("Overview Offset", &level.player.oversightCamera.offset[0], 2);
	ImGui::InputFloat("Overview FOV", &level.player.oversightCamera.FOV, 1);

	ImGui::InputFloat3("Arrow Position", &level.player.arrowCamera.position[0], 2);
	ImGui::InputFloat3("Arrow Direction", &level.player.arrowCamera.direction[0], 2);
	ImGui::InputFloat3("Arrow Offset", &level.player.arrowCamera.offset[0], 2);
	ImGui::InputFloat("Arrow FOV", &level.player.arrowCamera.FOV, 1);

	ImGui::InputFloat3("Replay Position", &level.player.replayCamera.position[0], 2);
	ImGui::InputFloat3("Replay Direction", &level.player.replayCamera.direction[0], 2);
	ImGui::InputFloat3("Replay Offset", &level.player.replayCamera.offset[0], 2);
	ImGui::InputFloat("Replay FOV", &level.player.replayCamera.FOV, 1);

	ImGui::End();
#endif
}

void EditorState::editorWindow()
{
#ifdef IMGUI
	ImGui::Begin("Level Window");
	if (ImGui::SliderFloat("Camera Speed", &camSpeed, 1.0f, 10.0f))
		freeMove->setSpeed(camSpeed);
	ImGui::End();
#endif
}

void EditorState::pauseGame(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_ESCAPE && ev->action == GLFW_PRESS) {

		this->pushState(new PauseState());
	}
	if (ev->key == GLFW_KEY_BACKSPACE) {
		if (ev->action == 1) {
			ImGui::GetIO().KeysDown[ImGuiKey_Backspace] = ImGuiKey_Backspace;
		}
		else {
			ImGui::GetIO().KeysDown[ImGuiKey_Backspace] = 0;
		}
	}
	if (ev->key == GLFW_KEY_LEFT) {
		if (ev->action == 1) {
			ImGui::GetIO().KeysDown[ImGuiKey_LeftArrow] = ImGuiKey_LeftArrow;
		}
		else {
			ImGui::GetIO().KeysDown[ImGuiKey_LeftArrow] = 0;
		}
	}
	if (ev->key == GLFW_KEY_RIGHT) {
		if (ev->action == 1) {
			ImGui::GetIO().KeysDown[ImGuiKey_RightArrow] = ImGuiKey_RightArrow;
		}
		else {
			ImGui::GetIO().KeysDown[ImGuiKey_RightArrow] = 0;
		}
	}
}