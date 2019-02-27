#pragma once

#include <string>

#include "glm/glm.hpp"

#include "Engine/Rendering/GLAbstraction/Shader.h"
#include "Engine/Rendering/GLAbstraction/Framebuffer.h"
#include "Engine/Rendering/GLAbstraction/UniformBuffer.h"
#include "Engine/Components/Camera.h"


class EntityShader : public Shader
{
public:
	EntityShader(const std::string & vertex, const std::string & fragment, Framebuffer* shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	~EntityShader();

	void bind();
	void updateMeshData(unsigned texId);

	virtual void update(const float& dt);

	float getTime();

private:
	float time;
	Camera** camera;
	Framebuffer* shadowBuffer;
	glm::mat4* lightSpaceMatrix;
};

