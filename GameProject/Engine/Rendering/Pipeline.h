#pragma once

#include "GLAbstraction/RenderingResources.h"
#include "GLAbstraction/Framebuffer.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Components/Camera.h"
#include "Light/LightManager.h"

class Entity;

struct DirectionLight {
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
	void prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen = false); // Old rendering
	void prePassDepthModel(const std::vector<Model*>& renderingModels, bool toScreen = false);

	/*
		Draw directly to the screen
	*/
	void drawToScreen(const std::vector<Entity*>& renderingList); // Old rendering
	void drawModelToScreen(const std::vector<Model*>& renderingModels);

	/*
		Draw to framebuffer color texture, nothing will be visible on screen unless you draw the texture to a quad
	*/
	Texture* drawToTexture(const std::vector<Entity*>& renderingList); // Old rendering
	Texture* drawModelToTexture(const std::vector<Model*>& renderingModels);

	/*
		Use texture to draw to quad which cover the whole screen
	*/
	void drawTextureToQuad(Texture* tex);

	/*
		Draws models using instancing - seperate drawing method from functions above
	*/
	void drawInstanced(Model * model);


	/*
		Generates depth texture for shadows, input entities who should give away shadows
	*/
	void calcDirLightDepth(const std::vector<Entity*>& renderingList); // Old rendering
	void calcDirLightDepthInstanced(const std::vector<Model*>& renderingModels);

	void setActiveCamera(Camera* camera);
	Camera* getActiveCamera();

	Framebuffer* getFbo();
	Framebuffer* getShadowFbo();

private:
	Camera * camera;
	unsigned int width, height;
	//unsigned int shadowWidth, shadowHeight;
	Framebuffer fbo;
	Framebuffer shadowFbo;
	//glm::mat4 lightSpaceMatrix;

	// Old rendering draw functions
	void draw(const std::vector<Entity*>& renderingList);					// Old rendering
	void draw(const std::vector<Entity*>& renderingList, Shader* shader);	// Old rendering
	void drawModel(Model * model, Shader* shader);							// Old rendering
	void drawModelPrePass(Model * model);									// Old rendering

	// New rendering draw functions
	void drawModelPrePassInstanced(Model * model);

	void updateFramebufferDimension(WindowResizeEvent* event);

	void prePassDepthOn();
	void prePassDepthOff();

	Shader* ZprePassShader;				// Old rendering
	Shader* testShader;					// Old rendering
	Shader* ZprePassShaderInstanced;
	Shader* entityShaderInstanced;
	Shader* quadShader;

	Model* quad;

	DirectionLight mainLight;

	LightManager lm;

	std::vector<UniformBuffer*> uniformBuffers;

	/*
		Generates uniform buffer with shaders uniform blocks size and data specified, bound to bindingpoint specified.
	*/
	void addUniformBuffer(unsigned bindingPoint, const unsigned shaderID, const char* blockName);
};

