#include "ReplayPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Config.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/GuidingPhase.h>
#include <Game/GameLogic/AimPhase.h>

// Gui
#include <Engine/GUI/FontManager.h>
#include <Engine/GUI/Panel.h>
#include <Engine/GUI/Button.h>

// Remove later
#include <Utils/Logger.h>

ReplayPhase::ReplayPhase(GuidingPhase* guidingPhase)
    :Phase((Phase*)guidingPhase)
{
    /*
		Create replay arrow
	*/
	replayArrow = level.entityManager->addTracedEntity("ArrowReplay");

	replayArrow->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

    replayArrow->getTransform()->setPosition(level.player.arrowCamera.position);
	replayArrow->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	// Copy arrow path from arrow guider to path treader
    ArrowGuider* oldArrowGuider = guidingPhase->getArrowGuider();

    PathTreader* arrow = new PathTreader(replayArrow, oldArrowGuider->getPath());
    arrow->startTreading();

    // Add path visualizer for debugging
    if (ENABLE_PATH_VISUALIZERS) {
        pathVisualizer = new PathVisualizer(replayArrow, level.entityManager);
        pathVisualizer->addPath(oldArrowGuider->getPath());
    }

    // Remove old arrow entity
    Entity* oldArrow = guidingPhase->getPlayerArrow();
    level.entityManager->removeTracedEntity(oldArrow->getName());

    // Set up the player camera
    freeCam = level.entityManager->addTracedEntity("FreeCam");

    Transform* camTransform = freeCam->getTransform();

	camTransform->setPosition(level.player.replayCamera.position);
	camTransform->setForward(level.player.replayCamera.direction);
    camTransform->resetRoll();

	Camera* camera = new Camera(freeCam, "Camera");
    camera->setFOV(level.player.replayCamera.FOV);
	camera->init();

	freeMove = new FreeMove(freeCam);

	// Reset targets
	level.targetManager->resetTargets();

    // Begin replaying playthrough
    level.replaySystem->startReplaying();

	this->timeLeft = level.scoreManager->getTotalTime();
	this->showGui = false;
	this->minimized = false;

	Display::get().getRenderer().setActiveCamera(camera);

    EventBus::get().subscribe(this, &ReplayPhase::handleKeyInput);
}

void ReplayPhase::update(const float & dt)
{
	this->timeLeft -= dt;
	if (this->timeLeft <= 0 && !this->showGui)
	{
		activateGui();
		this->showGui = true;
	}
}

Entity* ReplayPhase::getFreeCam() const
{
    return freeCam;
}

Entity* ReplayPhase::getReplayArrow() const
{
    return replayArrow;
}

PathVisualizer* ReplayPhase::getPathVisualizer() const
{
    return pathVisualizer;
}

void ReplayPhase::handleKeyInput(KeyEvent* event)
{
    if (event->action != GLFW_PRESS) {
        return;
    }

    if (event->key == GLFW_KEY_2) {
        EventBus::get().unsubscribe(this, &ReplayPhase::handleKeyInput);

        freeCam->removeComponent(freeMove->getName());

        // Stop replaying playthrough
        level.replaySystem->stopReplaying();

        // Begin camera transition to the arrow
        CameraSetting currentCamSettings;

        Transform* camTransform = freeCam->getTransform();

        currentCamSettings.position = camTransform->getPosition();
        currentCamSettings.direction = camTransform->getForward();
        currentCamSettings.offset = level.player.replayCamera.offset;
        currentCamSettings.FOV = level.player.replayCamera.FOV;

        CameraSetting newCamSettings = level.player.arrowCamera;

        this->setupTransition(currentCamSettings, newCamSettings);

        EventBus::get().subscribe(this, &ReplayPhase::transitionToAim);
    }
}

void ReplayPhase::transitionToAim(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &ReplayPhase::transitionToAim);

    Phase* guidingPhase = new AimPhase(this);
    changePhase(guidingPhase);
}

void ReplayPhase::activateGui()
{
	FontManager::addFont("aldo", "./Game/assets/fonts/aldo/aldo.ttf", 40);

	glm::uvec2 panelSize(500, 700);

	// Create outer panel
	Panel* bigPanel = new Panel();
	bigPanel->setSize(panelSize);
	bigPanel->setOption(GUI::CENTER_X);
	bigPanel->setOption(GUI::CENTER_Y);
	bigPanel->setColor({ 0.1f, 0.1f, 0.1f, 0.99f });
	this->bigPanel = bigPanel;
	level.gui->addPanel(bigPanel);

	// Create minimized panel/button
	Button* smallPanel = new Button();
	smallPanel->setSize({ 70, 50 });
	smallPanel->setOption(GUI::FLOAT_LEFT);
	smallPanel->setOption(GUI::FLOAT_UP);
	smallPanel->addText("Show", "aldo", { 0.9f, 0.9f, 0.9f, 1.0f });
	smallPanel->setColor({ 0.1f, 0.1f, 0.1f, 0.99f });
	this->smallPanel = smallPanel;
	level.gui->addPanel(smallPanel);
	
	// Add minimize button
	Button* button = new Button();
	button->setSize({ 30, 30 });
	button->setOption(GUI::FLOAT_RIGHT);
	button->setOption(GUI::FLOAT_UP);
	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);
	//button->setHoverColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	button->addText("-", "arial", { 1.0f, 1.0f, 1.0f, 1.0f });
	
	// assign callback function
	std::function<void(void)> callback = std::bind(&ReplayPhase::guiCallback, this);
	button->setCallback(callback);
	smallPanel->setCallback(callback);
	bigPanel->addChild(button);

	// Add text - Title
	bigPanel->addText("Score", "aldo", { 0.9f, 0.9f, 0.9f, 1.0f });
	unsigned width = bigPanel->getText(0)->getWidth();
	bigPanel->updateText(0, panelSize.x/2 - width/2, panelSize.y - 60);

	// Add text - Score
	bigPanel->addText("Score: " + std::to_string(level.scoreManager->getScore()), "aldo", { 0.9f, 0.9f, 0.9f, 1.0f });
	bigPanel->updateText(1, 20, panelSize.y - 140);

	// Add text - Targets
	std::string t = "Targets: " + std::to_string(level.scoreManager->getTargetsHit()) + "/" + std::to_string(level.targetManager->getTotalTargets());
	bigPanel->addText(t, "aldo", { 0.9f, 0.9f, 0.9f, 1.0f });
	bigPanel->updateText(2, 20, panelSize.y - 200);


	// Set initial state
	this->minimized = true;
	guiCallback();
}

void ReplayPhase::guiCallback()
{
	int width = Display::get().getWidth();
	if (this->minimized)
	{
		// Move smallPanel
		this->smallPanel->setOption(GUI::FLOAT_LEFT, -width);

		// Move bigPanel
		this->bigPanel->setOption(GUI::CENTER_X, 0);

		this->minimized = false;
	}
	else
	{
		// Move bigPanel
		this->bigPanel->setOption(GUI::CENTER_X, -width);

		// Move smallPanel
		this->smallPanel->setOption(GUI::FLOAT_LEFT, 0);

		this->minimized = true;
	}
}
