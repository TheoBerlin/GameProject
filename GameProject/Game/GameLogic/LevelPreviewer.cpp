#include "LevelPreviewer.h"

#include <Engine/Entity/Entity.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Level/ReplayParser.h>
#include <Utils/Logger.h>

LevelPreviewer::LevelPreviewer(EntityManager* entityManager)
    :elapsedTime(0.0f),
    replayLength(0.0f),
    levelParser(),
    level(),
    entityManager(entityManager),
    levelStructure(),
    targetManager(),
    collisionHandler(),
    replaySystem(),
    replayExists(false)
{
    this->targetManager = new TargetManager();

    level.levelName = "";
    level.scoreManager = nullptr;
    level.entityManager = entityManager;
    level.replaySystem = &replaySystem;
    level.levelStructure = &levelStructure;
    level.targetManager = targetManager;
    level.collisionHandler = &collisionHandler;
    level.lightManager = &lightManager;
    level.helpGUI = nullptr;
}

LevelPreviewer::~LevelPreviewer()
{
	if (level.levelName != "") {
		delete targetManager;
	}

    this->stopReplaying();

	Display::get().getRenderer().clearRenderingTargets();

	// Delete all loaded models
	ModelLoader::unloadAllModels();
}

void LevelPreviewer::setLevel(const std::string& levelName)
{
    // Read level
    levelParser.readLevel(levelName, this->level);

    // Start moving targets
    level.targetManager->resetTargets();

    // Renderer setup
    Renderer& renderer = Display::get().getRenderer();

	renderer.getPipeline()->setLightManager(level.lightManager);
	renderer.getPipeline()->setWallPoints(level.levelStructure->getWallPoints(), level.levelStructure->getWallGroupsIndex());

    // Create camera
    this->cameraEntity = entityManager->addTracedEntity("PreviewCamera");

    this->camera = new Camera(this->cameraEntity);

    glm::vec3 pos = level.player.oversightCamera.position;
    glm::vec3 dir = level.player.oversightCamera.direction;

    Transform* camTransform = cameraEntity->getTransform();

    camTransform->setPosition(level.player.oversightCamera.position);
    camTransform->setForward(level.player.oversightCamera.direction);

    this->camera->setFOV(level.player.oversightCamera.FOV);
    this->camera->setOffset(level.player.oversightCamera.offset);

    this->camera->init();

    renderer.setActiveCamera(this->camera);

    // Read level replay if there is one
    std::vector<CollisionReplay> collisions;
    std::vector<KeyPoint> arrowPath;

    this->replayExists = ReplayParser::readReplay(this->level, levelName, collisions, arrowPath);

    if (!replayExists) {
        LOG_INFO("No replay exists for the selected level");
    } else {
        LOG_INFO("Starting previewed level's replay");

        this->replayLength = arrowPath.back().t + 1.0f;

        // Create replay arrow
        this->replayArrow = this->level.entityManager->addTracedEntity("PreviewReplayArrow");

        Model * model = ModelLoader::loadModel("./Game/assets/Arrow.fbx", level.collisionHandler);
	    Display::get().getRenderer().addRenderingTarget(model, SHADERS::DEFAULT);

        this->replayArrow->setModel(model);

        this->pathTreader = new PathTreader(replayArrow, arrowPath);

        this->replaySystem.setCollisionReplays(collisions);

        // Start the replay
        this->replaySystem.startReplaying();
        this->pathTreader->startTreading();
    }

    renderer.initInstancing();
}

void LevelPreviewer::render()
{
    if (level.levelName == "") {
        return;
    }

    Display& display = Display::get();
	Renderer& renderer = display.getRenderer();

	renderer.drawAllInstanced();
}

void LevelPreviewer::update(float dt)
{
    if (level.levelName == "") {
        return;
    }

	ParticleManager::get().update(dt);

    // Update entities
	std::vector<Entity*>& entities = level.entityManager->getAll();

	for (unsigned int i = 0; i < entities.size(); i += 1) {
		entities[i]->update(dt);
	}

	/*
		Update shaders
	*/
	Display::get().getRenderer().updateShaders(dt);

    if (!this->replayExists) {
        return;
    }

    replaySystem.update(dt);

    // Restart replay if the end has been reached
    elapsedTime += dt;

    if (elapsedTime > replayLength) {
        elapsedTime = 0.0f;

        this->targetManager->resetTargets();

        pathTreader->startTreading();

        replaySystem.setReplayTime(this->level, this->pathTreader, this->replayArrow, 0.0f);
    }
}

Level& LevelPreviewer::getLevel()
{
    return this->level;
}

void LevelPreviewer::stopReplaying()
{
    this->replaySystem.stopReplaying();

    if (replayArrow) {
		bool arrowDeleted = this->entityManager->removeTracedEntity(replayArrow->getName());

        if (!arrowDeleted) {
            LOG_WARNING("Entity manager failed to delete preview replay arrow: [%s]", replayArrow->getName().c_str());
        }

		replayArrow = nullptr;
	}

    this->targetManager->resetTargets();
}
