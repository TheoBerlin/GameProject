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
#include "Lighting/LightManager.h"

class TrailShader;
class Entity;
class PostProcessShader;
struct TrailPointData;

struct RenderingTarget {
	bool prePass;
	bool castShadow;
	bool visible;

	Model* model;
};

enum SHADERS {
	DEFAULT = 0,
	DRONE_SHADER = 1,	// Requires a third vbo with colors bound to location 7.
	WALL = 2,			//  Requires a third vbo with scale bound to location 7.
	INFINITY_PLANE = 3,
	INFINITY_PLANE_PREPASS = 4,	//Used for cutout in depthbuffer 
	ROOF_PLANE = 5
};

enum SHADERS_POST_PROCESS {
	NO_FILTER = 0,
	BLUR_FILTER = 1,
	REWIND_FILTER = 2,
	SIZE
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
	void prePassDepthModel(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets, bool toScreen = false);

	/*
		Draw directly to the screen
	*/
	void drawModelToScreen(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets);

	/*
		Draw to framebuffer color texture, nothing will be visible on screen unless you draw the texture to a quad
	*/
	Texture* drawModelToTexture(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets);

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
	void calcDirLightDepthInstanced(const std::vector<std::pair<RenderingTarget, SHADERS>>& renderingTargets);

	/*
		Generates uniform buffer with shaders uniform blocks size and data specified, bound to bindingpoint specified.
	*/
	void addUniformBuffer(unsigned bindingPoint, const unsigned shaderID, const char* blockName);

	/*
		Draw trail
	*/
	void drawTrail();

	/*
		Draw everything that should glow and blurs it
	*/
	void glowPass();

	/*
		Updates shaders
	*/
	void updateShaders(const float& dt);
	void addCurrentLightManager(LightManager * lm);
	void createLight(glm::vec4 position, glm::vec4 intensity, int distance);
	void updateLight(int index, glm::vec4 position, glm::vec4 intensity, int distance);
	void removeLight(int index);

	/*
		Updates trail shader
	*/
	void updateTrail(const std::vector<TrailPointData>& pointData, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));

	void setActiveCamera(Camera* camera);
	Camera* getActiveCamera();

	void setWallPoints(const std::vector<glm::vec3>& wallPoints, const std::vector<int>& wallGroupsIndex);

	Framebuffer* getFbo();
	Framebuffer* getShadowFbo();
	Framebuffer* getPostProcessFbo();

private:
	Camera * camera;
	unsigned int width, height;
	Framebuffer fbo;
	Framebuffer postProcessFbo;
	Framebuffer shadowFbo;
	glm::mat4 lightSpaceMatrix;

	// New rendering draw functions
	void drawModelPrePassInstanced(Model * model);
	void updateFramebufferDimension(WindowResizeEvent* event);

	void prePassDepthOn();
	void prePassDepthOff();

	Shader* ZprePassShaderInstanced;
	Shader* quadShader;
	Shader* particleShader;
	TrailShader* trailShader;
	Shader* combineShader;

	std::vector<EntityShader*> entityShaders;
	std::vector<PostProcessShader*> postProcessShaders;

	void createQuad();
	Model* quad;

	LightManager * lightManager;

	std::vector<UniformBuffer*> uniformBuffers;

	struct LightBuffer {
		PointLight pointLights[10];
		int nrOfPointLights;
		glm::vec3 padding;
	} lightBuffer;
};
