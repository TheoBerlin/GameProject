#include "GuidingPhase.h"

#include <Engine/AssetManagement/ModelLoader.h>
#include <Engine/Components/Camera.h>
#include <Engine/Rendering/Display.h>
#include <Engine/Rendering/Renderer.h>
#include <Game/Components/ArrowGuider.h>

GuidingPhase::GuidingPhase(const Level& level, const glm::vec3& playerPos)
    :Phase(level)
{
	/*
		Create arrow entity
	*/
	Model * model = ModelLoader::loadModel("./Game/assets/Arrow.fbx");

	Entity * entity = level.entityManager->addTracedEntity("Player");

	entity->getTransform()->setPosition(playerPos);
	entity->getTransform()->setScale(glm::vec3(0.5f, 0.5f, 0.25f));

	entity->setModel(model);

	/*
		Add camera to arrow entity
	*/
	Camera* camera = new Camera(entity, "Camera", { 0.0f, 0.5f, -1.0f });
	camera->init();

	/*
		Add arrowguider to entity
	*/
	ArrowGuider* arrow = new ArrowGuider(entity, 2.0f);
	arrow->startGuiding();

	// Reset targets
	level.targetManager->resetTargets();

	Display::get().getRenderer().setActiveCamera(camera);
}

GuidingPhase::~GuidingPhase()
{
    level.entityManager->removeTracedEntity("Player");
}
