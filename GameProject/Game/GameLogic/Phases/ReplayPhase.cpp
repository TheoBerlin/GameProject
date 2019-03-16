#include "ReplayPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/FreeMove.h>
#include <Engine/Config.h>
#include <Engine/Events/EventBus.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Engine/Sound/SoundManager.h>
#include <Game/Components/ArrowGuider.h>
#include <Game/Components/PathVisualizer.h>
#include <Game/Components/TrailEmitter.h>
#include <Game/GameLogic/Phases/GuidingPhase.h>
#include <Game/GameLogic/Phases/AimPhase.h>
#include <Utils/Settings.h>
#include <Utils/Utils.h>

ReplayPhase::ReplayPhase(GuidingPhase* guidingPhase)
	:Phase((Phase*)guidingPhase),
	replayTime(0.0f)
{
    flightTime = guidingPhase->getFlightTime();

    // Create replay time bar
    setupGUI();

    flightTime = guidingPhase->getFlightTime();

    this->replaySpeedFactor = 1.0f;
    this->isPausing = false;

    // Create results window and minimize it
    // Lambda function which executes when retry is pressed
    std::function<void()> retry = [this](){beginAimTransition();};

	level.scoreManager->showResults(level, retry);

	level.scoreManager->toggleGuiMinimize();

	/*
		Create replay arrow
	*/
	replayArrow = level.entityManager->addTracedEntity("ArrowReplay");

	replayArrow->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

	Transform* arrowTransform = replayArrow->getTransform();

	arrowTransform->setPosition(level.player.arrowCamera.position);

	// Set the arrow's forward to point to the second keypoint in the path
	ArrowGuider* oldArrowGuider = guidingPhase->getArrowGuider();

	if (oldArrowGuider->getPath().size() > 1) {
		arrowTransform->setForward(glm::normalize(oldArrowGuider->getPath()[1].Position - oldArrowGuider->getPath()[0].Position));

		arrowTransform->resetRoll();
	}

	// Copy arrow path from arrow guider to path treader
	pathTreader = new PathTreader(replayArrow, oldArrowGuider->getPath());
	pathTreader->startTreading();

	// Add path visualizer for debugging
	if (ENABLE_PATH_VISUALIZERS) {
		pathVisualizer = new PathVisualizer(replayArrow, level.entityManager);
		pathVisualizer->addPath(oldArrowGuider->getPath());
	}

	//Retrieve trail emitter from playerArrow
	Entity* oldArrow = guidingPhase->getPlayerArrow();
	if (oldArrow) {
		TrailEmitter* trailEmitter = dynamic_cast<TrailEmitter*>(oldArrow->getComponent("TrailEmitter"));
		// Detach trail emitter from player arrow
		oldArrow->detachComponent("TrailEmitter");

		// Attach component to replay arrow
		trailEmitter->setHost(this->replayArrow);

		trailEmitter->resetTrailTimer();
	}

    // Remove old arrow entity
    level.entityManager->removeTracedEntity(oldArrow->getName());

	// Set up the player camera
	this->camera = new Camera(replayArrow, "Camera");
	this->camera->setFOV(level.player.arrowCamera.FOV);
	this->camera->init();

	this->camera->setPosition(transitionCam->getPosition());
	this->camera->setForward(transitionCam->getForward());

	// Create freemove and lock cursor
	this->thirdPersonController = new ThirdPersonController(replayArrow);

	glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Reset targets
	level.targetManager->resetTargets();

	// Begin replaying playthrough
	level.replaySystem->startReplaying();

	level.helpGUI->switchPhase(PHASE::REPLAY);

	Display::get().getRenderer().setActiveCamera(camera);

	EventBus::get().subscribe(this, &ReplayPhase::handleKeyInput);
	EventBus::get().subscribe(this, &ReplayPhase::handleMouseClick);
}

ReplayPhase::~ReplayPhase()
{
	EventBus::get().unsubscribe(this, &ReplayPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &ReplayPhase::finishAimTransition);
	EventBus::get().unsubscribe(this, &ReplayPhase::handleMouseClick);
}

void ReplayPhase::update(const float& dt)
{
    if (isPausing) {
        // Slow down time if pausing
        replaySpeedFactor = glm::max(replaySpeedFactor - dt * (1.0f / timeToPause), 0.0f);

    } else {
        // Increase speed factor back to 1.0f
        replaySpeedFactor = glm::min(replaySpeedFactor + dt * (1.0f / timeToPause), 1.0f);
    }

	SoundManager::get().setEffectsMasterPitch(replaySpeedFactor);

    // Make sure the player camera is always updated with dt, regardless of replay speed factor
    float cameraUpdateTime = dt + dt * (1.0f - replaySpeedFactor);

    if (thirdPersonController) {
        thirdPersonController->update(cameraUpdateTime);

        camera->update(cameraUpdateTime);
    } else if (freeCam) {
        freeCam->update(cameraUpdateTime);
    }

    float updateTime = dt * replaySpeedFactor;

    level.replaySystem->update(updateTime);

    // Advance time bar and slider
    if (replayTime < flightTime) {
        replayTime += updateTime;

		float replayProgress = replayTime / flightTime;

		if (this->timebarExists)
		{
			// Set timeBarFront and timeBarSlider new position
			glm::uvec2 timeBarSize = { 1 + screenWidth * (1 - timeBarSidePadding * 2) * replayProgress, timeBarHeightFactor * screenHeight };
			timeBarFront->setOption(GUI::FLOAT_RIGHT, (int)timeBarFront->getSize().y - (int)timeBarSize.x);

			timeBarSlider->setOption(GUI::FLOAT_LEFT, (int)timeBarSize.x - (int)timeBarSlider->getSize().x);
		}
    }

    ParticleManager::get().update(updateTime);

	// Update entities
    level.entityManager->update(updateTime);

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	/*
		Update shaders
	*/
	renderer.updateShaders(updateTime);
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
	if (event->key == GLFW_KEY_ESCAPE) {
		level.scoreManager->toggleGuiMinimize();
	}

	else if (event->key == GLFW_KEY_SPACE) {
		this->switchCamera();
	}
}

void ReplayPhase::handleMouseClick(MouseClickEvent* event)
{
	if (event->button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (event->action == GLFW_PRESS) {
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			if (thirdPersonController) {
				thirdPersonController->enableMouse();
			}
			else {
				freeMove->enableMouse();
			}
		}
		else {
			glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			if (thirdPersonController) {
				thirdPersonController->disableMouse();
			}
			else {
				freeMove->disableMouse();
			}
		}
	}
}

void ReplayPhase::beginAimTransition()
{
	this->timebarExists = false;

	EventBus::get().unsubscribe(this, &ReplayPhase::handleKeyInput);
	EventBus::get().unsubscribe(this, &ReplayPhase::handleMouseClick);

	// Reset time speed
	this->isPausing = false;
	this->replaySpeedFactor = 1.0f;

	// Remove results GUI if visible
	if (level.scoreManager->resultsVisible()) {
		level.scoreManager->removeResultsGUI(level);
	}

	// Reset score
	level.scoreManager->resetScore();

	// Set helpGUI for next phase
	level.helpGUI->switchPhase(PHASE::AIM);

    // Lock cursor
    glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Stop replaying playthrough
	level.replaySystem->stopReplaying();

	// Remove GUI elements
	level.gui->removePanel(backPanel);
    level.gui->removePanel(playPauseButton);

	// Begin camera transition to the arrow
	CameraSetting currentCamSettings;

	currentCamSettings.position = camera->getPosition();
	currentCamSettings.direction = camera->getForward();
	currentCamSettings.offset = { 0.0f, 0.0f, 0.0f };
	currentCamSettings.FOV = level.player.arrowCamera.FOV;

	CameraSetting newCamSettings = level.player.arrowCamera;

    // Transition above walls if the free cam is being used, otherwise transition backwards through the arrow's path
    if (freeCam) {
        this->transitionAboveWalls(currentCamSettings, newCamSettings);
    } else {
        std::vector<KeyPoint> path = pathTreader->getPath();

        // Start treading backwards from the treader's current position
        path.resize(pathTreader->getCurrentPointIndex() + 1);

        this->transitionBackwards(currentCamSettings, newCamSettings, path);
    }

	// Remove camera controller
	if (freeCam) {
		level.entityManager->removeTracedEntity(freeCam->getName());
		freeCam = nullptr;
	} else {
		replayArrow->removeComponent(thirdPersonController->getName());
		thirdPersonController = nullptr;
	}

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
	this->timebarExists = true;

	backPanel = new Panel();
	timeBarBack = new Button();
	timeBarFront = new Panel();

	// Position the time bar panels
	this->screenWidth = (unsigned)Settings::get().getScreenWidth();
	this->screenHeight = (unsigned)Settings::get().getScreenHeight();

	glm::uvec2 timeBarPos = { screenWidth * timeBarSidePadding, screenHeight * timeBarBottomPadding };

	timeBarBack->setOption(GUI::FIT_X);
	timeBarBack->setOption(GUI::FIT_Y);
	backPanel->setPosition(timeBarPos);

	// Size the time bars so that the back panel covers nearly the entire width
	// and the front panel is invisible
	glm::uvec2 timeBarSize = { screenWidth * (1 - timeBarSidePadding * 2), timeBarHeightFactor * screenHeight };
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
	timeBarBack->setCallback([this](void) {handleTimeBarClick(); });

	// Add panels to GUI
	backPanel->addChild(timeBarBack);
	backPanel->addChild(timeBarFront);

	// Add slider to time bar
	timeBarSlider = new Panel();

	// Size the button
	this->sliderSize = { sliderSizeFactors.x * screenHeight, sliderSizeFactors.y * screenHeight };

	timeBarSlider->setSize(sliderSize);

	// Position the slider
	timeBarSlider->setOption(GUI::FLOAT_LEFT);

	// Set button visuals
	timeBarSlider->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// Add button to GUI
	backPanel->addChild(timeBarSlider);

	// Add the parent panel to level GUI
	level.gui->addPanel(backPanel);

    // Create play/pause button
    playPauseButton = new Button();

    // Horizontal size
    glm::vec2 playPauseSize = {screenHeight * playPauseSizeFactor * playPauseAspect, screenHeight * playPauseSizeFactor};

    playPauseButton->setSize(glm::uvec2((unsigned)playPauseSize.x, (unsigned)playPauseSize.y));

    // Place the button in the middle of the screen, slightly above the time bar
    glm::uvec2 playPausePos = {(unsigned)screenWidth / 2 - playPauseSize.x / 2, timeBarPos.y + timeBarSize.y * 2};

    playPauseButton->setPosition(playPausePos);

    playPauseButton->setNormalColor(timeBarBackColor);
    playPauseButton->setHoverColor(timeBarBackColor * 1.1f);
    playPauseButton->setPressedColor(timeBarFrontColor);

    playPauseButton->setCallback([this](){this->handlePlayPause();});

    Texture* playPauseTx = TextureManager::loadTexture("./Game/Assets/playPause.png");

    playPauseButton->setBackgroundTexture(playPauseTx);

    level.gui->addPanel(playPauseButton);
	// Add collision marks to bar
	addCollisionMarks();
}

void ReplayPhase::addCollisionMarks()
{
	std::vector<CollisionReplay> v = level.replaySystem->getCollisionReplays();

	for (auto t : v)
	{
		float replayProgress = t.time / this->flightTime;
		unsigned int width = (unsigned)(this->backPanel->getSize().x * replayProgress);
		Panel* p = new Panel();
		p->setOption(GUI::SCALE_TEXTURE_TO_HEIGHT, (int)this->timeBarSlider->getSize().y);
		p->setColor({ 1.f, 1.f, 1.f, 1.f });
		p->setBackgroundTexture(TextureManager::getTexture("./Game/Assets/droneIcon.png"));
		p->setOption(GUI::FLOAT_LEFT, width);
		this->backPanel->addChild(p);
	}
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

	//Update trail emitter timer
	TrailEmitter* trailEmitter = dynamic_cast<TrailEmitter*>(replayArrow->getComponent("TrailEmitter"));

	if (trailEmitter)
		trailEmitter->setTrailTimer(this->replayTime);
}	

void ReplayPhase::handlePlayPause()
{
    this->isPausing = !this->isPausing;
}

void ReplayPhase::switchCamera()
{
	if (freeCam) {
		// Switch to third person camera
		this->camera->setHost(replayArrow);

		this->thirdPersonController = new ThirdPersonController(replayArrow);

		level.entityManager->removeTracedEntity(freeCam->getName());

		freeCam = nullptr;
	}
	else {
		// Switch to freecam
		this->freeCam = level.entityManager->addTracedEntity("FreeCam");

		// Set freecam's position and forward
		Transform* transform = freeCam->getTransform();

		transform->setPosition(camera->getPosition());
		transform->setForward(camera->getForward());
		transform->resetRoll();

		glm::vec3 r = transform->getRight();

		this->freeMove = new FreeMove(freeCam);
		this->freeMove->disableMouse();

		this->camera->setHost(freeCam);
		this->camera->couple();

		this->replayArrow->removeComponent(this->thirdPersonController->getName());

		this->thirdPersonController = nullptr;
	}
}