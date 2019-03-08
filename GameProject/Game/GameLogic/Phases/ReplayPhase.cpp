#include "ReplayPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Config.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/GameLogic/Phases/GuidingPhase.h>
#include <Game/GameLogic/Phases/AimPhase.h>
#include <Utils/Settings.h>

ReplayPhase::ReplayPhase(GuidingPhase* guidingPhase)
    :Phase((Phase*)guidingPhase),
    replayTime(0.0f)
{
    // GUI setup
    setupGUI();

    flightTime = guidingPhase->getFlightTime();

    /*
		Create replay arrow
	*/
	replayArrow = level.entityManager->addTracedEntity("ArrowReplay");

	replayArrow->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

    replayArrow->getTransform()->setPosition(level.player.arrowCamera.position);
	replayArrow->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	// Copy arrow path from arrow guider to path treader
    ArrowGuider* oldArrowGuider = guidingPhase->getArrowGuider();

    pathTreader = new PathTreader(replayArrow, oldArrowGuider->getPath());
    pathTreader->startTreading();

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

    // Create freemove and lock cursor
	freeMove = new FreeMove(freeCam);
    freeMove->enableMouse();

    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Reset targets
	level.targetManager->resetTargets();

    // Begin replaying playthrough
    level.replaySystem->startReplaying();

	Display::get().getRenderer().setActiveCamera(camera);

    EventBus::get().subscribe(this, &ReplayPhase::handleKeyInput);
}

ReplayPhase::~ReplayPhase()
{
	EventBus::get().unsubscribe(this, &ReplayPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &ReplayPhase::finishAimTransition);
}

void ReplayPhase::update(const float& dt)
{
    level.replaySystem->update(dt);

    // Advance time bar and slider
    if (replayTime < flightTime) {
        replayTime += dt;

        float replayProgress = replayTime/flightTime;

		// Set timeBarFront and timeBarSlider new position
        glm::uvec2 timeBarSize = {1 + screenWidth * (1 - timeBarSidePadding * 2) * replayProgress, timeBarHeightFactor * screenHeight};
		timeBarFront->setOption(GUI::FLOAT_RIGHT, (int)timeBarFront->getSize().y - (int)timeBarSize.x);

		timeBarSlider->setOption(GUI::FLOAT_LEFT, (int)timeBarSize.x - (int)timeBarSlider->getSize().x);
    }


    // Display results when the replay finishes
    if (replayTime > flightTime && !level.scoreManager->resultsVisible())
    {
        // Disable freemove and unlock cursor
        glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        freeMove->disableMouse();

        // Lambda function which executes when retry is pressed
        std::function<void()> retry = [this](){beginAimTransition();};

        level.scoreManager->showResults(level, retry);
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

    // Minimize / enlarge results GUI
    if (event->key == GLFW_KEY_ESCAPE && level.scoreManager->resultsVisible()) {
        level.scoreManager->toggleGuiMinimize();
    }

    else if (event->key == GLFW_KEY_2) {
        beginAimTransition();
    } else if (event->key == GLFW_KEY_C) {
        // Toggle mouse lock
        if (freeMove->mouseIsEnabled()) {
            glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        freeMove->toggleMouse();
    }
}

void ReplayPhase::beginAimTransition()
{
    EventBus::get().unsubscribe(this, &ReplayPhase::handleKeyInput);

    // Remove freecam
    freeCam->removeComponent(freeMove->getName());

    // Lock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Remove results GUI if visible
    if (level.scoreManager->resultsVisible()) {
        level.scoreManager->removeResultsGUI(level);
    }

    // Stop replaying playthrough
    level.replaySystem->stopReplaying();

	// Remove GUI elements
	level.gui->removePanel(backPanel);

    // Begin camera transition to the arrow
    CameraSetting currentCamSettings;

    Transform* camTransform = freeCam->getTransform();

    currentCamSettings.position = camTransform->getPosition();
    currentCamSettings.direction = camTransform->getForward();
    currentCamSettings.offset = level.player.replayCamera.offset;
    currentCamSettings.FOV = level.player.replayCamera.FOV;

    CameraSetting newCamSettings = level.player.arrowCamera;

    this->setupTransition(currentCamSettings, newCamSettings);

    EventBus::get().subscribe(this, &ReplayPhase::finishAimTransition);
}

void ReplayPhase::finishAimTransition(CameraTransitionEvent* event)
{
    EventBus::get().unsubscribe(this, &ReplayPhase::finishAimTransition);

    Phase* guidingPhase = new AimPhase(this);
    changePhase(guidingPhase);
}

void ReplayPhase::setupGUI()
{
	backPanel = new Panel();
    timeBarBack = new Button();
    timeBarFront = new Panel();

    // Position the time bar panels
    this->screenWidth = (unsigned)Settings::get().getScreenWidth();
    this->screenHeight = (unsigned)Settings::get().getScreenHeight();

    glm::uvec2 timeBarPos = {screenWidth * timeBarSidePadding, screenHeight * timeBarBottomPadding};

	timeBarBack->setOption(GUI::FIT_X);
	timeBarBack->setOption(GUI::FIT_Y);
	backPanel->setPosition(timeBarPos);

    // Size the time bars so that the back panel covers nearly the entire width
    // and the front panel is invisible
    glm::uvec2 timeBarSize = {screenWidth * (1 - timeBarSidePadding * 2), timeBarHeightFactor * screenHeight};
    timeBarBack->setSize(timeBarSize);
	backPanel->setSize(timeBarSize);

    // Make the front time bar to be as small as possible
    timeBarFront->setSize(timeBarSize);
	timeBarFront->setOption(GUI::FLOAT_LEFT);

    // Set panel visuals
    timeBarBack->setNormalColor(timeBarBackColor);
    timeBarBack->setHoverColor(timeBarBackColor);
    timeBarBack->setPressedColor(timeBarBackColor);

	backPanel->setColor({ 0.0f, 0.0f, 0.0f, 0.0f });

    timeBarFront->setColor(timeBarFrontColor);

    // Handle mouse clicks on the time bar
    timeBarBack->setCallback([this](void) {handleTimeBarClick();});

    // Add panels to GUI
	backPanel->addChild(timeBarBack);
	backPanel->addChild(timeBarFront);

    // Add slider to time bar
    timeBarSlider = new Panel();

    // Size the button
    this->sliderSize = {sliderSizeFactors.x * screenHeight, sliderSizeFactors.y * screenHeight};

    timeBarSlider->setSize(sliderSize);

    // Position the slider
	timeBarSlider->setOption(GUI::FLOAT_LEFT);

    // Set button visuals
    timeBarSlider->setColor({1.0f, 1.0f, 1.0f, 1.0f});

    // Add button to GUI
	backPanel->addChild(timeBarSlider);

	// Add the parent panel to level GUI
	level.gui->addPanel(backPanel);
}

void ReplayPhase::handleTimeBarClick()
{
    // Get horizontal mouse coordinate
    double posX;

    glfwGetCursorPos(Display::get().getWindowPtr(), &posX, nullptr);

    // Calculate the desired time to rewind to
    unsigned int timeBarBegin = (unsigned)(screenWidth * timeBarSidePadding);
    unsigned int timeBarEnd = screenWidth - timeBarBegin;

    float desiredFactor = ((float)posX - timeBarBegin) / (timeBarEnd - timeBarBegin);

    float desiredTime = desiredFactor * flightTime;

    level.replaySystem->setReplayTime(level, pathTreader, freeCam, desiredTime);

    replayTime = desiredTime;
}
