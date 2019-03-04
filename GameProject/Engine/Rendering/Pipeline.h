#pragma once

#include "GLAbstraction/RenderingResources.h"
#include "GLAbstraction/Framebuffer.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Components/Camera.h"
#include "Engine/Particle/ParticleManager.h"
#include "Engine/Rendering/GLAbstraction/VertexArray.h"
#include "Engine/Rendering/GLAbstraction/VertexBuffer.h"


class Entity;
class PostProcessShader;

struct DirectionalLight {
	glm::vec4 direction;
	glm::vec4 color_intensity;
};

enum SHADERS {
	DEFAULT = 0,
	DRONE_SHADER = 1, // Requires a third vbo with colors bound to location 7.
};

enum SHADERS_POST_PROCESS {
	NO_FILTER = 0,
	BLUR_FILTER = 1, 
};

class Pipeline
{
public:
	Pipeline();
	~Pipeline();

	/*
		Used for rendering particles
	*/
	Texture* drawParticle();

	/*
		PrePassDepth will stop any draw calls from writing to the depth buffer. Everything drawn in this pass will be used for depth testing
	*/
	void prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen = false); // Old rendering
	void prePassDepthModel(const std::vector<std::pair<Model*, SHADERS>>& renderingModels, bool toScreen = false);

	/*
		Draw directly to the screen
	*/
	void drawToScreen(const std::vector<Entity*>& renderingList); // Old rendering
	void drawModelToScreen(const std::vector<std::pair<Model*, SHADERS>>& renderingModels);

	/*
		Draw to framebuffer color texture, nothing will be visible on screen unless you draw the texture to a quad
	*/
	Texture* drawToTexture(const std::vector<Entity*>& renderingList); // Old rendering
	Texture* drawModelToTexture(const std::vector<std::pair<Model*, SHADERS>>& renderingModels);

	Texture* combineTextures(Texture* sceen, Texture* particles);

	/*
		Use texture to draw to quad which cover the whole screen
	*/
	void drawTextureToQuad(Texture* tex, SHADERS_POST_PROCESS shader = SHADERS_POST_PROCESS::NO_FILTER, bool drawToFBO = false);

	/*
		Draws models using instancing - seperate drawing method from functions above
	*/
	void drawInstanced(Model * model, SHADERS shader = SHADERS::DEFAULT);

	/*
		Generates depth texture for shadows, input entities who should give away shadows
	*/
	void calcDirLightDepth(const std::vector<Entity*>& renderingList); // Old rendering
	void calcDirLightDepthInstanced(const std::vector<std::pair<Model*, SHADERS>>& renderingModels);

	/*
		Generates uniform buffer with shaders uniform blocks size and data specified, bound to bindingpoint specified.
	*/
	void addUniformBuffer(unsigned bindingPoint, const unsigned shaderID, const char* blockName);

	/*
		Updates shaders
	*/
	void updateShaders(const float& dt);

	void setActiveCamera(Camera* camera);
	Camera* getActiveCamera();

	Framebuffer* getFbo();
	Framebuffer* getShadowFbo();

private:
	Camera * camera;
	unsigned int width, height;
	unsigned int shadowWidth, shadowHeight;
	Framebuffer fbo;
	Framebuffer shadowFbo;
	glm::mat4 lightSpaceMatrix;

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
	Shader* quadShader;
	Shader* particleShader;
	Shader* combineShader;

	std::vector<EntityShader*> entityShaders;
	std::vector<PostProcessShader*> postProcessShaders;

	void createQuad();
	Model* quad;

	DirectionalLight mainLight;

	std::vector<UniformBuffer*> uniformBuffers;
};

