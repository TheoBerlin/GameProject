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

	this->renderingModels.push_back(ModelLoader::loadModel("./Game/assets/Cube.fbx"));
	this->renderingModels.push_back(ModelLoader::loadModel("./Game/assets/floor.fbx"));
	this->renderingModels.push_back(ModelLoader::loadModel("./Game/assets/Arrow.fbx"));
	this->renderingModels.push_back(ModelLoader::loadModel("./Game/assets/droneTarget.fbx"));
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
}

void Renderer::updateInstancingData(Model * model)
{
	model->updateInstancingData();
}

void Renderer::drawAllInstanced()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->pipeline.getFbo()->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (Model* model : this->renderingModels) {
		this->pipeline.drawInstanced(model);
	}

	this->pipeline.getFbo()->unbind();

	this->pipeline.drawTextureToQuad(this->pipeline.getFbo()->getColorTexture(0));
}

void Renderer::drawModel(Model * model)
{
	glDisable(GL_DEPTH_TEST);
	this->pipeline.drawInstanced(model);
}

