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

	freeMove = new FreeMove(freeCam);

	// Reset targets
	level.targetManager->resetTargets();

    // Begin replaying playthrough
    level.replaySystem->startReplaying();

	Display::get().getRenderer().setActiveCamera(camera);

    EventBus::get().subscribe(this, &ReplayPhase::handleKeyInput);
}

void ReplayPhase::update(const float& dt)
{
    level.replaySystem->update(dt);

    // Advance time bar and slider
    if (replayTime < flightTime) {
        replayTime += dt;

        float replayProgress = replayTime/flightTime;

        glm::uvec2 timeBarSize = {1 + screenWidth * (1 - timeBarSidePadding * 2) * replayProgress, timeBarHeightFactor * screenHeight};
        timeBarFront->setSize(timeBarSize);

        glm::uvec2 sliderPos = {timeBarSidePadding * screenWidth + timeBarSize.x - sliderSize.x / 2, screenHeight * timeBarBottomPadding};
        timeBarSlider->setPosition(sliderPos);
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

    if (event->key == GLFW_KEY_1) {
        level.replaySystem->rewindLevel(level, pathTreader, flightTime / 2.0f);

        replayTime = flightTime / 2.0f;
    }

    if (event->key == GLFW_KEY_2) {
        EventBus::get().unsubscribe(this, &ReplayPhase::handleKeyInput);

        // Teardown
        freeCam->removeComponent(freeMove->getName());

        // Stop replaying playthrough
        level.replaySystem->stopReplaying();

        // Remove GUI elements
        level.gui->removePanel(timeBarSlider);
        level.gui->removePanel(timeBarFront);
        level.gui->removePanel(timeBarBack);

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

void ReplayPhase::setupGUI()
{
    timeBarBack = new Panel();
    timeBarFront = new Panel();

    // Position the time bar panels
    this->screenWidth = (unsigned)Settings::get().getScreenWidth();
    this->screenHeight = (unsigned)Settings::get().getScreenHeight();

    glm::uvec2 timeBarPos = {screenWidth * timeBarSidePadding, screenHeight * timeBarBottomPadding};

    timeBarBack->setPosition(timeBarPos);
    timeBarFront->setPosition(timeBarPos);

    // Size the time bars so that the back panel covers nearly the entire width
    // and the front panel is invisible
    glm::uvec2 timeBarSize = {screenWidth * (1 - timeBarSidePadding * 2), timeBarHeightFactor * screenHeight};
    timeBarBack->setSize(timeBarSize);

    timeBarSize.x = 1;
    timeBarFront->setSize(timeBarSize);

    // Set panel visuals
    timeBarBack->setColor({0.9686f, 0.7725f, 0.2039f, 1.0f});
    timeBarFront->setColor({0.6588f, 0.4784f, 0.0f, 1.0f});

    // Add panels to GUI
    level.gui->addPanel(timeBarBack);
    level.gui->addPanel(timeBarFront);

    // Add slider to time bar
    timeBarSlider = new Button();

    // Size the button
    this->sliderSize = {sliderSizeFactors.x * screenHeight, sliderSizeFactors.y * screenHeight};

    timeBarSlider->setSize(sliderSize);

    // Position the slider
    timeBarSlider->setPosition({timeBarPos.x - sliderSize.x / 2, timeBarPos.y + 20});

    // Set button visuals
    timeBarSlider->setColor({1.0f, 1.0f, 1.0f, 1.0f});

    // Add button to GUI
    level.gui->addPanel(timeBarSlider);
}
