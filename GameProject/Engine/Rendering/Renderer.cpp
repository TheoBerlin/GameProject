#include "Renderer.h"

#include "../Entity/Entity.h"
#include "../AssetManagement/Mesh.h"
#include "GLAbstraction/Texture.h"
#include "Utils/TimeProfiler.h"

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
	postProcessTexture = this->pipeline.drawToTexture(this->renderingList);

	tex = this->pipeline.drawParticle();
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

	Model * model = ModelLoader::loadModel("./Game/assets/Arrow.fbx");
	this->addRenderingTarget(model, SHADERS::DEFAULT);

	/*
		Initilize colors vertexBuffer for collision color changing
	*/
	model = ModelLoader::loadModel("./Game/assets/droneTarget.fbx");
	AttributeLayout layout;
	layout.push(3, 1); // vec3 color which can be changed seperately for each entity;
	std::vector<glm::vec3> colors;
	for (size_t i = 0; i < model->getRenderingGroup().size(); i++)
		colors.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	if(colors.size() > 0)
		model->initInstancing(0, (void*)&colors[0][0], colors.size() * sizeof(glm::vec3), layout);
}

void Renderer::clearRenderingTargets()
{
	this->renderingTargets.clear();
}

void Renderer::updateInstancingData(Model * model)
{
	model->updateInstancingData();
}

void Renderer::drawAllInstanced()
{
	//TimeProfiler tp("test", true);
	//tp.startInterval("draw");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	Calulate shadow depth
	*/
	this->pipeline.calcDirLightDepthInstanced(this->renderingTargets);

	/*
		Z-prepass stage
	*/
	this->pipeline.prePassDepthModel(this->renderingTargets);

	/*
		Drawing stage with pre existing depth buffer to texture
	*/
	Texture* postProcess = this->pipeline.drawModelToTexture(this->renderingTargets);

	pipeline.drawParticle();
	
	postProcessTexture = pipeline.getFbo()->getColorTexture(0);
	tex = pipeline.getFbo()->getColorTexture(1);

	Texture* combinedTex = pipeline.combineTextures(postProcessTexture, tex);

	/*
		Draw texture of scene to quad for postprocessing
	*/
	this->pipeline.drawTextureToQuad(combinedTex);

	//tp.endInterval();
	//tp.endFrame();
}

void Renderer::updateShaders(const float & dt)
{
	this->pipeline.updateShaders(dt);
}

void Renderer::drawTextureToScreen(Texture * texture, SHADERS_POST_PROCESS shader)
{
	/*
		Draw texture of scene to quad for postprocessing
	*/
	this->pipeline.drawTextureToQuad(texture, shader);
}

Texture* Renderer::drawTextureToFbo(Texture * texture, SHADERS_POST_PROCESS shader)
{
	/*
		Draw texture of color attachment and return that color attachment
	*/
	this->pipeline.drawTextureToQuad(texture, shader, true);

	return this->pipeline.getFbo()->getColorTexture(0);
}

Pipeline * Renderer::getPipeline()
{
	return &this->pipeline;
}

void Renderer::addRenderingTarget(Model * model, SHADERS shader, bool castShadow, bool prePass, bool visible)
{
	RenderingTarget rt;

	rt.prePass = prePass;
	rt.castShadow = castShadow;
	rt.visible = visible;

	if (model) {
		rt.model = model;

		this->renderingTargets.push_back(std::make_pair(rt, shader));
	}
}



