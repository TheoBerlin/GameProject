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

	/*
	Push entity to the rendering list.
	Arguments:
		entity: The entity to be rendered.
	*/
	void push(Entity* entity);

	/*
	Push the manager to draw all particles
	*/
	void pushParticleManager(ParticleManager* particleManager);

	/*
	Draw all entites in the rendering list and clear it.
	*/
	void drawAll();

	/*
		inits vertex buffer for meshes rendering groups transforms
	*/
	void initInstancing();

	/*
		Updates vertex buffer for models meshes rendering groups transforms
	*/
	void updateInstancingData(Model * model);


	/*
		Draw all entites in the rendering list, assumes that all entities in rendering list uses the same model
	*/
	void drawAllInstanced();


private:
	Pipeline pipeline;

	ParticleManager* particleManager;
	Texture postProcessTexture;
	Texture tex;

	std::vector<Entity*> renderingList;

};