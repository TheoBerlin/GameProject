#pragma once

#include "../Components/Camera.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Pipeline.h"

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
	Push entity to the rendering list.
	Arguments:
		entity: The entity to be rendered.
	*/
	void push(Entity* entity);

	/*
		Draw all entites in the rendering list and clear it.
	*/
	void drawAll();

	/*
		inits vertex buffer for meshes rendering groups transforms and create renderingModels list
	*/
	void initInstancing();

	/*
		clears renderingModels list
	*/
	void clearRenderingModels();

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


private:
	Pipeline pipeline;

	std::vector<Entity*> renderingList;
	std::vector<std::pair<Model*, SHADERS>> renderingModels;

};