#include "Renderer.h"

#include "../Entity/Entity.h"
#include "../AssetManagement/Mesh.h"
#include "GLAbstraction/Texture.h"
#include <Utils/Logger.h>

Renderer::Renderer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	this->activePostFilters.resize(SHADERS_POST_PROCESS::SIZE);

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

	/*
		Initilize colors vertexBuffer for collision color changing
	*/
	model = ModelLoader::loadModel("./Game/assets/droneTargetMoving.fbx");
	if (colors.size() > 0)
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
		Calulate shadow depth
	*/
	this->pipeline.calcDirLightDepthInstanced(this->renderingTargets);

	/*
		Z-prepass stage
	*/
	this->pipeline.prePassDepthModel(this->renderingTargets, false, SHADERS::DRONE_GHOST);

	/*
		Drawing stage with pre existing depth buffer to texture
	*/
	this->postProcessTexture = this->pipeline.drawModelToTexture(this->renderingTargets, SHADERS::DRONE_GHOST);

	/*
		Draw drones
	*/
	this->pipeline.drawModelsWithShader(this->postProcessTexture, this->renderingTargets, SHADERS::DRONE_GHOST);

	/*
		Draw trail
	*/
	this->pipeline.drawTrail();

	/*
		Draw Particle
	*/
	pipeline.drawParticle();

	this->postProcessTexture = pipeline.getFbo()->getColorTexture(0);
	tex = pipeline.getFbo()->getColorTexture(1);

	/*
		Apply blur to glow texture
	*/
	this->pipeline.drawTextureToQuad(tex, SHADERS_POST_PROCESS::BLUR_FILTER, true);
	Texture* blurTexture = this->getPipeline()->getPostProcessFbo()->getColorTexture(0);
	
	/*
		Combine scene texture with glow texture
	*/
	Texture* combinedTex = pipeline.combineTextures(postProcessTexture, blurTexture);



	/*
		Go through post process effects then draw texture to screen
	*/
	this->postProcessTexture = this->getPipeline()->getPostProcessFbo()->getColorTexture(0);
	bool firstPostProcess = true;
	for (unsigned i = 1; i < this->activePostFilters.size(); i++) {
		if (this->activePostFilters[i]) {
			if (firstPostProcess) {
				this->pipeline.drawTextureToQuad(combinedTex, static_cast<SHADERS_POST_PROCESS>(i), true);
				firstPostProcess = false;
			}
			else {
				this->pipeline.drawTextureToQuad(this->postProcessTexture, static_cast<SHADERS_POST_PROCESS>(i), true);
			}
		}
	}

	if(firstPostProcess)
		this->pipeline.drawTextureToQuad(combinedTex);
	else
		this->pipeline.drawTextureToQuad(this->postProcessTexture);
	
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

	return this->pipeline.getPostProcessFbo()->getColorTexture(0);
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

void Renderer::activatePostFilter(SHADERS_POST_PROCESS shader)
{
	if (shader == SHADERS_POST_PROCESS::NO_FILTER) {
		LOG_WARNING("Illegal post process shader attempted to be activated (NO_FILTER)");
		return;
	}

	if ((unsigned)shader > (unsigned)0 && (unsigned)shader < this->activePostFilters.size())
		activePostFilters[shader] = true;
}

void Renderer::deactivatePostFilter(SHADERS_POST_PROCESS shader)
{
	if (shader == SHADERS_POST_PROCESS::NO_FILTER) {
		LOG_WARNING("Illegal post process shader attempted to be deactivated (NO_FILTER)");
		return;
	}

	if((unsigned)shader > (unsigned)0 && (unsigned)shader < this->activePostFilters.size())
		activePostFilters[shader] = false;
}
