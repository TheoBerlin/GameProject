#include "LevelPreviewer.h"

#include <Engine/Entity/Entity.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>

LevelPreviewer::LevelPreviewer(EntityManager* entityManager)
    :elapsedTime(0.0f),
    replayLength(0.0f),
    levelParser(),
    level(),
    entityManager(entityManager),
    levelStructure(),
    targetManager(),
    collisionHandler()
{
    this->targetManager = new TargetManager();

    level.levelName = "";
    level.scoreManager = nullptr;
    level.entityManager = entityManager;
    level.levelStructure = &levelStructure;
    level.targetManager = targetManager;
    level.collisionHandler = &collisionHandler;
    level.lightManager = &lightManager;
}

LevelPreviewer::~LevelPreviewer()
{
	if (level.levelName != "") {
		delete targetManager;
	}

	Display::get().getRenderer().clearRenderingTargets();

	// Delete all loaded models
	ModelLoader::unloadAllModels();
}

void LevelPreviewer::setLevel(const std::string& levelName)
{
    // Read level
    levelParser.readLevel(levelName, this->level);

    // Renderer setup
    Renderer& renderer = Display::get().getRenderer();

    renderer.initInstancing();
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
}

Level& LevelPreviewer::getLevel()
{
    return this->level;
}
