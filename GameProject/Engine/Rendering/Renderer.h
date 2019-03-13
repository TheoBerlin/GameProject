#pragma once

#include "../Components/Camera.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Pipeline.h"

#include "Engine/Particle/ParticleManager.h"

class Model;
class Entity;
class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	/*
	Set the camera which the shader will use as the source of the projection and view matrices.
	Arguments:
		camera: The new active camera.
	*/
	void setActiveCamera(Camera* camera);

	Camera* getActiveCamera();

	/*
		inits vertex buffer for meshes rendering groups transforms and create renderingTargets list
	*/
	void initInstancing();

	/*
		clears renderingTargets list
	*/
	void clearRenderingTargets();

	/*
		Updates vertex buffer for models meshes rendering groups transforms
	*/
	void updateInstancingData(Model * model);

	/*
		Draw all entites in the rendering list, assumes that all entities in rendering list uses the same model
	*/
	void drawAllInstanced();

	/*
		Updates all shaders who need updates
	*/
	void updateShaders(const float & dt);

	/*
		Draw texture to screen using a post process  shader
	*/
	void drawTextureToScreen(Texture * texture, SHADERS_POST_PROCESS shader);
	
	/*
		Draw texture to fbo color attachment 0 using a post process  shader
	*/
	Texture* drawTextureToFbo(Texture * texture, SHADERS_POST_PROCESS shader);

	/*
		Get pipeline
	*/
	Pipeline* getPipeline();

	/*
		Adds rendering target, tells renderer to draw a model with a specfic shader
	*/
	void addRenderingTarget(Model* model, SHADERS shader = SHADERS::DEFAULT, bool castShadow = true, bool prePass = true, bool visible = true);

private:
	Pipeline pipeline;

	ParticleManager* particleManager;
	Texture* postProcessTexture;
	Texture* tex;

	std::vector<std::pair<RenderingTarget, SHADERS>> renderingTargets;

};