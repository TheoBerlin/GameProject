#include "EditorState.h"

#include <Engine/States/StateManager.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/GUI/GUI.h>
#include <Engine/Rendering/GUIRenderer.h>

#include "Engine/Entity/Transform.h"

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

	level.entityManager = &this->entityManager;
	level.targetManager = targetManager;
	level.scoreManager = &this->scoreManager;
	level.collisionHandler = &this->collisionHandler;
	level.levelStructure = &this->levelStructure;
	level.lightManager = &this->lightManager;
	level.gui = &this->getGUI();
	level.isEditor = true;

	Display::get().getRenderer().getPipeline()->setLightManager(level.lightManager);

	EventBus::get().subscribe(this, &EditorState::pauseGame);
	InputHandler ih(Display::get().getWindowPtr());

	ImGui::GetIO().KeyMap[ImGuiKey_Backspace] = ImGuiKey_Backspace;

	freeMove->disableMouse();

	Model* model;
	model = ModelLoader::loadModel("./Game/assets/lampPlaceholder.fbx");
	model->setName("lampPlaceholder");
	Display::get().getRenderer().addRenderingTarget(model);

	lampPlacerholder = new Entity();
	lampPlacerholder->setModel(model);
	lampPlacerholder->getTransform()->setScale(0.0f);
	lampPlacerholder->setName("Placeholder");
}

EditorState::~EditorState()
{
	delete targetManager;
	EventBus::get().unsubscribe(this, &EditorState::pauseGame);

	Display::get().getRenderer().clearRenderingTargets();

	// Delete all loaded models
	ModelLoader::unloadAllModels();

	delete this->lampPlacerholder;
}

void EditorState::start()
{
	/*
	All entities in this state puts themselves in the rendering group of their model
	*/
	std::vector<Entity*>& entities = this->entityManager.getAll();
	for (Entity* entity : entities)
		entity->attachToModel();
}

void EditorState::end()
{
	/*
	All entities removes themselves from the rendering group of their model
	*/
	std::vector<Entity*>& entities = this->entityManager.getAll();
	for (Entity* entity : entities)
		entity->detachFromModel();
}

void EditorState::update(const float dt)
{
	freeMove->update(dt);
	camera.update(dt);

	// Update entities.
	std::vector<Entity*>& entities = this->entityManager.getAll();

	for (unsigned int i = 0; i < entities.size(); i++) {
		entities[i]->update(dt);
	}

	mainWindow(entityManager);

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();
}

void EditorState::render()
{
	/*
EntityManager& entityManager = this->entityManager;
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
	ImGui::SetWindowSize({ 110, 180 });

	if (ImGui::Button("Level"))
		activeWindow[0] = !activeWindow[0];
	if (ImGui::Button("Entities"))
		activeWindow[1] = !activeWindow[1];
	if (ImGui::Button("Walls"))
		activeWindow[2] = !activeWindow[2];
	if (ImGui::Button("Light"))
		activeWindow[3] = !activeWindow[3];
	if (ImGui::Button("Player"))
		activeWindow[4] = !activeWindow[3];
	if (ImGui::Button("Editor/Help"))
		activeWindow[5] = !activeWindow[5];
	ImGui::NewLine();

	ImGui::End();
#endif

	if (activeWindow[0])
		levelWindow(entityManager);
	if (activeWindow[1])
		entityWindow(entityManager);
	if (activeWindow[2])
		wallWindow(entityManager);
	if (activeWindow[3])
		lightWindow(entityManager);
	if (activeWindow[4])
		playerWindow(entityManager);
	if (activeWindow[5])
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
			if (entityManager.getEntity(i)->getName().substr(0, 9) != "WallPoint" && entityManager.getEntity(i)->getName().substr(0, 11) != "Placeholder") {
				if (ImGui::Selectable(entityManager.getEntity(i)->getName().c_str(), is_selected)) {
					currentItem = entityManager.getEntity(i)->getName();
					currentModel = entityManager.getEntity(i)->getModel()->getName();
					currentEntity = i;
					currentIsTarget = false;
					for (unsigned int j = 0; j < targetManager->getMovingTargets().size(); j++) {
						if (entityManager.getEntity(i) == targetManager->getMovingTargets()[j].pathTreader->getHost()) {
							currentIsTarget = true;
							break;
						}
					}
					for (unsigned int j = 0; j < targetManager->getStaticTargets().size(); j++) {
						if (entityManager.getEntity(i) == targetManager->getStaticTargets()[j].hoverAnimation->getHost()) {
							currentIsTarget = true;
							break;
						}
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}
	if (ImGui::Button("Add Entity")) {
		Entity* newEntity = new Entity();
		newEntity->setName("Entity" + std::to_string(entityManager.getEntitySize()));
		Model* model = ModelLoader::loadModel("./Game/assets/Cube.fbx");
		newEntity->setModel(model);
		Display::get().getRenderer().addRenderingTarget(model);
		newEntity->getModel()->setName("Cube");
		entityManager.addEntity(newEntity);
		currentModel = newEntity->getModel()->getName();
		currentEntity = entityManager.getEntitySize() - 1;
		currentIsTarget = false;
	}
	
	if (currentEntity != -1) {
		if (ImGui::Button("Duplicate Entity")) {
			Entity* dupeEntity = entityManager.getEntity(currentEntity);
			Entity* newEntity = new Entity();

			Transform* newTrans = newEntity->getTransform();
			Transform* dupeTransform = dupeEntity->getTransform();

			newTrans->setPosition(dupeTransform->getPosition());
			newTrans->setScale(dupeTransform->getScale());
			newTrans->setRotationQuat(dupeTransform->getRotationQuat());

			if (currentIsTarget) {
				newEntity->setName("Target " + std::to_string(level.targetManager->getTargetCount()));
				level.targetManager->addStaticTarget(newEntity, newEntity->getTransform()->getPosition());
			}
			else {
				newEntity->setName("Entity" + std::to_string(entityManager.getEntitySize()));
				currentIsTarget = false;
			}
				

			Model* dupeModel = dupeEntity->getModel();
			newEntity->setModel(dupeModel);
			newEntity->getModel()->setName(dupeModel->getName());

			entityManager.addEntity(newEntity);

			currentModel = newEntity->getModel()->getName();
			currentEntity = entityManager.getEntitySize() - 1;
			
		}
	}

	if (ImGui::Button("Remove Entity")) {
		if (currentIsTarget) {
			for (unsigned int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
				if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
					level.targetManager->removeTarget(entityManager.getEntity(currentEntity)->getName());
				}
			}
			for (unsigned int i = 0; i < level.targetManager->getStaticTargets().size(); i++) {
				if (level.targetManager->getStaticTargets()[i].hoverAnimation->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
					level.targetManager->removeTarget(entityManager.getEntity(currentEntity)->getName());
				}
			}
		}
		entityManager.removeEntity(currentEntity);
		currentEntity = -1;
	}
	if (currentEntity != -1) {
		Entity* curEntity = entityManager.getEntity(currentEntity);
		//Entity Info
		std::string name = curEntity->getName();
		glm::vec3 position = curEntity->getTransform()->getPosition();
		glm::vec3 scale = curEntity->getTransform()->getScale();
		glm::vec3 rotation = curEntity->getTransform()->getYawPitchRoll();

		ImGui::Text("Entity Info");
		if (ImGui::InputText("Name", &name[0], 64))
			curEntity->setName(name.c_str());
		if(ImGui::DragFloat3("Position", &position[0], 0.1f))
			curEntity->getTransform()->setPosition(position);
		if (ImGui::DragFloat3("Scale", &scale[0], 0.1f))
			curEntity->getTransform()->setScale(scale);
		if (ImGui::DragFloat3("Rotation", &rotation[0], 0.1f))
			curEntity->getTransform()->setRotation(rotation);
		if (ImGui::RadioButton("IsTarget", currentIsTarget)) {
			if (!currentIsTarget)
				level.targetManager->addStaticTarget(curEntity, curEntity->getTransform()->getPosition());
			else
				level.targetManager->removeTarget(curEntity->getName());
			currentIsTarget = !currentIsTarget;
		}
		if (currentIsTarget) {
			ImGui::SameLine();
			if (ImGui::Button("Add Path")) {
				bool found = false;
				for (unsigned int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
					if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
						std::vector<KeyPoint> path = level.targetManager->getMovingTargets()[i].pathTreader->getPath();
						Transform * currEntityTransform = entityManager.getEntity(currentEntity)->getTransform();
						float timeStamp = glm::length(currEntityTransform->getPosition() - path.back().Position) / 3.0f + path.back().t;
						level.targetManager->addKeyPoint(entityManager.getEntity(currentEntity), KeyPoint(currEntityTransform->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f), timeStamp));
						found = true;
					}
				}
				if (!found) {
					for (unsigned int i = 0; i < level.targetManager->getStaticTargets().size(); i++) {
						if (level.targetManager->getStaticTargets()[i].hoverAnimation->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
							Transform * currEntityTransform = entityManager.getEntity(currentEntity)->getTransform();
							std::vector<KeyPoint> path;
							path.push_back(KeyPoint(currEntityTransform->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f));
							path.push_back(KeyPoint(currEntityTransform->getPosition() + glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f));
							level.targetManager->removeTarget(entityManager.getEntity(currentEntity)->getName());
							level.targetManager->addMovingTarget(entityManager.getEntity(currentEntity), path);
						}
					}
				}
			}

			if (level.targetManager->getMovingTargets().size() > 0) {
				ImGui::SameLine();
				if (ImGui::Button("Remove Path")) {
					for (unsigned int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
						if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {
							std::vector<KeyPoint> path = level.targetManager->getMovingTargets()[i].pathTreader->getPath();
							path.pop_back();
							level.targetManager->getMovingTargets()[i].pathTreader->setPath(path);
							if (level.targetManager->getMovingTargets()[i].pathTreader->getPath().size() == 0) {
								level.targetManager->addStaticTarget(level.targetManager->getMovingTargets()[i].pathTreader->getHost(), glm::vec3(0.0f));
								level.targetManager->removeTarget(level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName());
							}
						}
					}
				}
			}
			for (unsigned int i = 0; i < level.targetManager->getMovingTargets().size(); i++) {
				if (level.targetManager->getMovingTargets()[i].pathTreader->getHost()->getName() == entityManager.getEntity(currentEntity)->getName()) {

					std::vector<KeyPoint> path = level.targetManager->getMovingTargets()[i].pathTreader->getPath();
					for (unsigned int j = 0; j < level.targetManager->getMovingTargets()[i].pathTreader->getPath().size(); j++) {
						if (ImGui::DragFloat3(std::string("Path " + std::to_string(j)).c_str(), &path[j].Position[0], 0.1f))
							level.targetManager->getMovingTargets()[i].pathTreader->setPath(path);
						ImGui::SameLine();
						ImGui::PushItemWidth(50);
						if(ImGui::DragFloat(std::string(std::string("Time stamp") + std::to_string(j)).c_str(), &path[j].t, 0.1f))
							level.targetManager->getMovingTargets()[i].pathTreader->setPath(path);
						ImGui::PopItemWidth();
					}
				}
			}
		}
		//Model Info
		Model* model = curEntity->getModel();
		ImGui::Text("Model Info");
		ImGui::InputText("Model Name", &currentModel[0], 128);
		if (ImGui::Button("Load Model")) {

			size_t index = currentModel.find('\0');
			if(index != std::string::npos)
				currentModel.erase(index, currentModel.size() - index);

			std::string path = std::string("./Game/assets/") + currentModel + ".fbx";
			if (!ModelLoader::getModel(path.c_str())) {
				model = ModelLoader::loadModel(std::string("./Game/assets/") + currentModel.c_str() + ".fbx");
				Display::get().getRenderer().addRenderingTarget(model);
				curEntity->setModel(model);
				curEntity->getModel()->setName(currentModel.c_str());
			}
			else {
				model = ModelLoader::getModel(std::string("./Game/assets/") + currentModel.c_str() + ".fbx");
				curEntity->setModel(model);
				curEntity->getModel()->setName(currentModel.c_str());
			}
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
		levelParser.writeLevel(std::string("./Game/Level/Levels/") + levelName.c_str() + ".json", level);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		entityManager.removeEntities();
		freeMove->disableMouse();

		levelParser.readLevel(std::string("./Game/Level/Levels/") + levelName.c_str() + ".json", level);
		Display::get().getRenderer().getPipeline()->setLightManager(level.lightManager);
		//Get directional light settings
		this->dirLightDirection = lightManager.getDirectionalLight()->getDirection();
		this->dirLightColorIntesity = lightManager.getDirectionalLight()->getIntensity();

		Display::get().getRenderer().initInstancing();
		entityManager.addEntity(lampPlacerholder);
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

	if (ImGui::Button("Set Overview settings to current camera settings")) {
		CameraSetting& oc = level.player.oversightCamera;
		Transform* cameraTransform = this->camera.getTransform();
		oc.position = cameraTransform->getPosition();
		oc.direction = cameraTransform->getForward();
	}

	ImGui::InputFloat3("Arrow Position", &level.player.arrowCamera.position[0], 2);
	ImGui::InputFloat3("Arrow Direction", &level.player.arrowCamera.direction[0], 2);

	if (ImGui::Button("Set Arrow settings to current camera settings")) {
		CameraSetting& ac = level.player.arrowCamera;
		Transform* cameraTransform = this->camera.getTransform();
		ac.position = cameraTransform->getPosition();
		ac.direction = cameraTransform->getForward();
	}

	float optimalTime = level.scoreManager->getOptimalTime();
	if (ImGui::InputFloat("Optimal Time", &optimalTime, 1, 1, 2))
		level.scoreManager->setOptimalTime(optimalTime);

	ImGui::End();
#endif
}

void EditorState::wallWindow(EntityManager & entityManager)
{
#ifdef IMGUI
	ImGui::Begin("Wall Window");
	
	static float wallHeight = level.levelStructure->getWallHeight();
	

	if (ImGui::DragFloat("Wall Height:", &wallHeight, 0.1f)) {
		level.levelStructure->setWallHeight(wallHeight);
		level.levelStructure->updateBuffers();
	}


	if (ImGui::Button("Add Wall")) {
		level.levelStructure->addWall(level);
	}
	std::string wallTex = level.levelStructure->getTexture();
	if (ImGui::InputText("Wall Texture", (char*)wallTex.c_str(), wallTex.capacity())) {
		level.levelStructure->setTexture(wallTex.c_str());
	}
	if (ImGui::BeginCombo("Wall Group", currentWall.c_str())) {
		for (unsigned int i = 0; i < level.levelStructure->getWallGroupsIndex().size(); i++) {
			bool is_selected = (currentWall == std::to_string(i)); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(std::to_string(i).c_str(), is_selected)) {
				currentWall = std::to_string(i);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	if (std::stoi(currentWall) != -1) {
		unsigned wallGroupIndex = (unsigned)std::stoi(currentWall);
		static int InputIndex = 0;
		int wallCount = level.levelStructure->getWallGroupsIndex()[wallGroupIndex];

		ImGui::InputInt("Input index:", &InputIndex, 1);

		InputIndex = (unsigned)InputIndex % wallCount;

		if (ImGui::Button("Add Point")) {
			level.levelStructure->addPoint(level, std::stoi(currentWall), (unsigned)InputIndex);
		}
		if (ImGui::Button("Remove Point")) {
			int prevSize = level.levelStructure->getWallGroupsIndex().size();
			level.levelStructure->removePoint(level, wallGroupIndex, InputIndex);
			if (level.levelStructure->getWallGroupsIndex().size() != prevSize)
				currentWall = "-1";
		}
		else {
			unsigned offset = 0;

			std::vector<int>& wallGroupsIndex = level.levelStructure->getWallGroupsIndex();
			for (unsigned int i = 0; i < wallGroupIndex; i++)
				offset += wallGroupsIndex[i];
			for (unsigned int i = 0; i < wallCount; i++) {
				glm::vec2 position = glm::vec2(level.levelStructure->getWallPoints()[i + offset].x, level.levelStructure->getWallPoints()[i + offset].z);
				if (ImGui::DragFloat2(std::string("Point " + std::to_string(i)).c_str(), &position[0], 0.1f)) {
					level.levelStructure->editPoint(level, wallGroupIndex, i + offset, glm::vec3(position.x, 0, position.y));
				}
			}
		}
	}
	ImGui::End();
#endif
}

void EditorState::lightWindow(EntityManager & entityManager)
{
#ifdef IMGUI
	ImGui::Begin("Light Window");
	ImGui::Text("Directional Light Settings");
	if (ImGui::DragFloat4("Direction", &this->dirLightDirection[0], 0.1f) ||
		ImGui::DragFloat4("ColorIntensity", &this->dirLightColorIntesity[0], 0.1f)) {
		level.lightManager->updateDirectionalLight(this->dirLightDirection, this->dirLightColorIntesity, &level);
		Display::get().getRenderer().getPipeline()->updateDirectionalLight(&DirectionalLight(this->dirLightDirection, this->dirLightColorIntesity));
	}

	if (ImGui::Button("Add Light")) {
		Display::get().getRenderer().getPipeline()->createLight(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), 7);
	}
	if (ImGui::BeginCombo("Light Group", currentLight.c_str())) {
		for (unsigned int i = 0; i < level.lightManager->getNrOfPointLights(); i++) {
			bool is_selected = (currentLight == std::to_string(i)); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(std::to_string(i).c_str(), is_selected)) {
				currentLight = std::to_string(i);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
		}
		ImGui::EndCombo();
	}
	if (currentLight != "-1") {
		if (ImGui::Button("Remove Light")) {
			Display::get().getRenderer().getPipeline()->removeLight(std::stoi(currentLight));
			currentLight = "-1";
		}
		else {
			glm::vec3 position = level.lightManager->getPointLights()->at(std::stoi(currentLight))->getPosition();
			glm::vec4 colour = level.lightManager->getPointLights()->at(std::stoi(currentLight))->getIntensity();
			int distance = level.lightManager->getPointLights()->at(std::stoi(currentLight))->getDistance();

			for (unsigned int i = 0; i < entityManager.getEntitySize(); i++) {
				if (entityManager.getEntity(i)->getName().substr(0, 11) == "Placeholder") {
					entityManager.getEntity(i)->getTransform()->setPosition(position);
					entityManager.getEntity(i)->getTransform()->setScale(0.1f);
				}
			}

			bool change = false;
			if (ImGui::DragFloat3("Position", &position[0], 0.1f))
				change = true;
			if (ImGui::DragFloat4("Color + Intensity", &colour[0], 0.1f))
				change = true;
			if (ImGui::DragInt("Distance", &distance, 1))
				change = true;
			if(change)
				Display::get().getRenderer().getPipeline()->updateLight(std::stoi(currentLight), glm::vec4(position, 1.0f), colour, distance);
		}
	}
	ImGui::End();
#endif
}

void EditorState::editorWindow()
{
#ifdef IMGUI
	ImGui::Begin("Editor Window");
	if (ImGui::SliderFloat("Camera Speed", &camSpeed, 1.0f, 10.0f))
		freeMove->setSpeed(camSpeed);
	ImGui::Text("Press 'F2' to switch camera mode");
	ImGui::NewLine();
	ImGui::Text("Don't load a level twice.\nIf you want to load a new level exit and enter the editor again.");
	ImGui::End();
#endif
}

void EditorState::pauseGame(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_ESCAPE && ev->action == GLFW_PRESS) {

		this->pushState(new PauseState());
	}
	if (ev->key == GLFW_KEY_F2 && ev->action == GLFW_PRESS) {
		if (freeMove->mouseIsEnabled()) {
			freeMove->disableMouse();
			freeMove->setSpeed(0.0f);
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			freeMove->enableMouse();
			freeMove->setSpeed(this->camSpeed);
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	if (ev->key == GLFW_KEY_BACKSPACE) {
		if (ev->action) {
			ImGui::GetIO().KeysDown[ImGuiKey_Backspace] = true;
		}
		else {
			ImGui::GetIO().KeysDown[ImGuiKey_Backspace] = false;
		}
	}
	if (ev->key == GLFW_KEY_LEFT) {
		if (ev->action) {
			ImGui::GetIO().KeysDown[ImGuiKey_LeftArrow] = true;
		}
		else {
			ImGui::GetIO().KeysDown[ImGuiKey_LeftArrow] = false;
		}
	}
	if (ev->key == GLFW_KEY_RIGHT) {
		if (ev->action) {
			ImGui::GetIO().KeysDown[ImGuiKey_RightArrow] = true;
		}
		else {
			ImGui::GetIO().KeysDown[ImGuiKey_RightArrow] = false;
		}
	}
}
