#include "EntityShader.h"
#include <math.h>

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
	
	this->setCameraUniform();
	this->setCamPosUniform();
	this->setLightSpaceMatrixUniform();
	this->setShadowBufferUniform();
}

void EntityShader::updateMeshData(unsigned texId)
{
	Shader::setTexture2D("tex", 0, texId);
}

void EntityShader::setCameraUniform(const std::string& uniformName)
{
	Shader::setUniformMatrix4fv(uniformName, 1, false, &((*this->camera)->getVP()[0][0]));
}

void EntityShader::setShadowBufferUniform(const std::string& uniformName)
{
	Texture * shadowTex = this->shadowBuffer->getDepthTexture();
	Shader::setTexture2D(uniformName, 1, shadowTex->getID());
}

void EntityShader::setLightSpaceMatrixUniform(const std::string& uniformName)
{
	Shader::setUniformMatrix4fv(uniformName, 1, false, &((*this->lightSpaceMatrix)[0][0]));
}

void EntityShader::setCamPosUniform(const std::string & uniformName)
{
	Shader::setUniform3fv("camPos", 1, &(*this->camera)->getPosition()[0]);
}

void EntityShader::update(const float & dt)
{
	this->time += dt;
	this->time = fmod(this->time, 20000.0f);
}

float EntityShader::getTime()
{
	return this->time;
}

