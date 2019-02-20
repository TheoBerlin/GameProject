#pragma once

#include "GLAbstraction/RenderingResources.h"
#include "GLAbstraction/Framebuffer.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Components/Camera.h"

class Entity;

struct DirectionalLight {
	glm::vec4 direction;
	glm::vec4 color_intensity;
};

class Pipeline
{
public:
	Pipeline();
	~Pipeline();
	/*
		PrePassDepth will stop any draw calls from writing to the depth buffer. Everything drawn in this pass will be used for depth testing
	*/
	void prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen = false);

	/*
		Draw directly to the screen
	*/
	void drawToScreen(const std::vector<Entity*>& renderingList);

	/*
		Draw to framebuffer color texture, nothing will be visible on screen unless you draw the texture to a quad
	*/
	Texture* drawToTexture(const std::vector<Entity*>& renderingList);
	/*
		Use texture to draw to quad which cover the whole screen
	*/
	void drawTextureToQuad(Texture* tex);

	/*
		Draws models using instancing - seperate drawing method from functions above
	*/
	void drawInstanced(Model * model);

	void drawAllInstanced();

	/*
		Generates depth texture for shadows, input entities who should give away shadows
	*/
	void calcDirLightDepth(const std::vector<Entity*>& renderingList /*,const glm::vec3& lightDir*/);

	void setActiveCamera(Camera* camera);

	Framebuffer* getFbo();
	Framebuffer* getShadowFbo();

private:
	Camera * camera;
	unsigned int width, height;
	unsigned int shadowWidth, shadowHeight;
	Framebuffer fbo;
	Framebuffer shadowFbo;
	glm::mat4 lightSpaceMatrix;

	void draw(const std::vector<Entity*>& renderingList);
	void draw(const std::vector<Entity*>& renderingList, Shader* shader);
	void drawModel(Model * model, Shader* shader);
	void drawModelPrePass(Model * model);

	void updateFramebufferDimension(WindowResizeEvent* event);

	void prePassDepthOn();
	void prePassDepthOff();

	Shader* ZprePassShader;
	Shader* testShader;
	Shader* entityShaderInstanced;
	Shader* quadShader;

	Model* quad;

	DirectionalLight mainLight;

	std::vector<UniformBuffer*> uniformBuffers;

	/*
		Generates uniform buffer with shaders uniform blocks size and data specified, bound to bindingpoint specified.
	*/
	void addUniformBuffer(unsigned bindingPoint, const unsigned shaderID, const char* blockName);
};

