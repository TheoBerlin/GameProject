#include "GameLogic.h"

#include "Engine/Rendering/Display.h"
#include "../Engine/Components/Component.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/Components/ArrowGuider.h"
#include "Engine/AssetManagement/ModelLoader.h"

GameLogic::GameLogic(EntityManager * em)
{
	this->em = em;
	this->currentPhase = 0;

	Entity * entity = this->em->addTracedEntity("Player");
	entity->getTransform()->setPosition(glm::vec3(1.0f, 1.0f, 0.5f));
	entity->getTransform()->setScale(glm::vec3(1.0f, 1.0f, 0.5f));

	Camera* camera = new Camera(entity, "Player", { 0.0f, 0.5f, -1.0f });
	camera->init();

	entity->setModel(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));

	ArrowGuider* arrow = new ArrowGuider(entity, 3.0f);
	arrow->startGuiding();

	//Display::get().getRenderer().setActiveCamera(camera);
}


GameLogic::~GameLogic()
{
}
