#include "Renderer.h"

#include "../Entity/Entity.h"
#include "../AssetManagement/Mesh.h"
#include "GLAbstraction/Texture.h"

Renderer::Renderer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

Renderer::~Renderer()
{

}

void Renderer::setActiveCamera(Camera * camera)
{
	this->pipeline.setActiveCamera(camera);
}

Camera * Renderer::getActiveCamera()
{
	return this->pipeline.getActiveCamera();
}

void Renderer::push(Entity * entity)
{
	this->renderingList.push_back(entity);
}

void Renderer::drawAll()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	this->pipeline.calcDirLightDepth(this->renderingList);
	/*
		Z-prepass stage
	*/
	this->pipeline.prePassDepth(this->renderingList);

	/*
		Drawing stage with pre existing depth buffer to texture
	*/
	Texture * postProcessTexture = this->pipeline.drawToTexture(this->renderingList);

	/*
		Draw texture of scene to quad for postprocessing
	*/
	this->pipeline.drawTextureToQuad(postProcessTexture);

	this->renderingList.clear();
}

void Renderer::initInstancing()
{
	std::vector<Model*> models = ModelLoader::getModels();

	for (Model* model : models) {
		model->initInstancing();
	}

	this->renderingModels.push_back(std::make_pair(ModelLoader::loadModel("./Game/assets/Cube.fbx"), SHADERS::DEFAULT));
	this->renderingModels.push_back(std::make_pair(ModelLoader::loadModel("./Game/assets/floor.fbx"), SHADERS::DEFAULT));
	this->renderingModels.push_back(std::make_pair(ModelLoader::loadModel("./Game/assets/Arrow.fbx"), SHADERS::DEFAULT));


	Model * model = ModelLoader::loadModel("./Game/assets/droneTarget.fbx");
	this->renderingModels.push_back(std::make_pair(model, SHADERS::DRONE_SHADER));
	/*
		Initilize colors vertexBuffer for collision color changing
	*/
	AttributeLayout layout;
	layout.push(3, 1); // vec3 color which can be changed seperately for each entity;
	std::vector<glm::vec3> colors;
	for (size_t i = 0; i < model->getRenderingGroup().size(); i++)
		colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	model->initInstancing(0, (void*)&colors[0][0], colors.size() * sizeof(glm::vec3), layout);
}

void Renderer::updateInstancingData(Model * model)
{
	model->updateInstancingData();
}

void Renderer::drawAllInstanced()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
		Calulate shadow depth
	*/
	this->pipeline.calcDirLightDepthInstanced(this->renderingModels);

	/*
		Z-prepass stage
	*/
	this->pipeline.prePassDepthModel(this->renderingModels);
	
	/*
		Drawing stage with pre existing depth buffer to texture
	*/
	Texture * postProcessTexture = this->pipeline.drawModelToTexture(this->renderingModels);

	/*
		Draw texture of scene to quad for postprocessing
	*/
	this->pipeline.drawTextureToQuad(postProcessTexture);
}

void Renderer::updateShaders(const float & dt)
{
	this->pipeline.updateShaders(dt);
}



