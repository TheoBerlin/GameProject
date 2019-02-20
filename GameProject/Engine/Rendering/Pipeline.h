#pragma once

#include "GLAbstraction/RenderingResources.h"
#include "GLAbstraction/Framebuffer.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Components/Camera.h"

//TEST TEST TEST REMOVE
#include "Engine/Particle/ParticleManager.h"
#include "Engine/Rendering/GLAbstraction/VertexArray.h"
#include "Engine/Rendering/GLAbstraction/VertexBuffer.h"

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

	//PARTICLE TEST
	Texture* drawParticle();


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
	void drawTextureToQuad(Texture* tex, Texture* texture);

	/*
		Draws models using instancing - seperate drawing method from functions above
	*/
	void drawInstanced(Model * model);

	/*
		Generates depth texture for shadows, input entities who should give away shadows
	*/
	Texture* calcDirLightDepth(const std::vector<Entity*>& renderingList, const glm::vec3& lightDir);

	void setActiveCamera(Camera* camera);

	Framebuffer* getFbo();

private:
	static const GLfloat g_vertex_buffer_data[16];

	Camera * camera;
	unsigned width, height;
	Framebuffer fbo;

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
	Shader* particleShader;
	bool p = true;
	GLuint billboard_vertex_buffer;
	GLuint particleDataBuffer;

	VertexArray va;
	VertexBuffer* vbBillboard;
	VertexBuffer* vbParticle;

	Model* quad;

	DirectionalLight mainLight;

	std::vector<UniformBuffer*> uniformBuffers;

	/*
		Generates uniform buffer with shaders uniform blocks size and data specified, bound to bindingpoint specified.
	*/
	void addUniformBuffer(unsigned bindingPoint, const unsigned shaderID, const char* blockName);
};

