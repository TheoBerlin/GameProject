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
	virtual ~EntityShader();

	void bind();
	void updateMeshData(unsigned texId);
	void updateLightMatrixData(glm::mat4 * lightSpaceMatrix);

	void setCameraUniform(const std::string& uniformName = "vp");
	void setShadowBufferUniform(const std::string& uniformName = "shadowTex");
	void setLightSpaceMatrixUniform(const std::string& uniformName = "lightMatrix");
	void setCamPosUniform(const std::string& uniformName = "camPos");

	virtual void update(const float& dt);

	float getTime();

private:
	float time;
	Camera** camera;
	Framebuffer* shadowBuffer;
	glm::mat4* lightSpaceMatrix;
};

