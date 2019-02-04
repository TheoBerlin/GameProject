#pragma once

#include "../Components/Camera.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"

class Model;
class Entity;
class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void setActiveCamera(Camera* camera);

	void draw(Entity* entity);
	void draw(Model* model);

private:
	Camera* activeCamera;
	UniformBuffer* uniformBuffer;
	Shader* testShader;
};