#include "EntityShader.h"

EntityShader::EntityShader(const std::string & vertex, const std::string & fragment, Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix) : Shader(vertex, fragment)
{
	this->camera = camera;
	this->shadowBuffer = shadowBuffer;
	this->lightSpaceMatrix = lightSpaceMatrix;
}

EntityShader::~EntityShader()
{
}

void EntityShader::bind()
{
	Shader::bind();
	
	Shader::setUniformMatrix4fv("lightMatrix", 1, false, &((*this->lightSpaceMatrix)[0][0]));
	Shader::setUniformMatrix4fv("vp", 1, false, &((*this->camera)->getVP()[0][0]));
	Shader::setUniform3fv("camPos", 1, &(*this->camera)->getPosition()[0]);

	Texture * shadowTex = this->shadowBuffer->getDepthTexture();
	Shader::setTexture2D("shadowTex", 1, shadowTex->getID());
}

void EntityShader::updateMeshData(unsigned texId)
{
	Shader:setTexture2D("tex", 0, texId);
}

void EntityShader::update(const float & dt)
{
	this->time += dt;
}

float EntityShader::getTime()
{
	return this->time;
}

