#include "DroneShader.h"

#include "Engine/AssetManagement/TextureManager.h"

DroneShader::DroneShader(const std::string & vertex, const std::string & fragment, Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix)
	: EntityShader(vertex, fragment, shadowBuffer, camera, lightSpaceMatrix)
{
	this->noiseTexture = TextureManager::loadTexture("./Game/assets/textures/noise.jpg");

	this->noiseTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	this->noiseTexture->unbind();
}

DroneShader::~DroneShader()
{
}

void DroneShader::bind()
{
	EntityShader::bind();
	Shader::setTexture2D("noiseTex", 2, noiseTexture->getID());
	Shader::setUniform1f("time", EntityShader::getTime());
}

void DroneShader::updateMeshData(unsigned texId)
{
	EntityShader::updateMeshData(texId);
}

void DroneShader::update(const float & dt)
{
	EntityShader::update(dt);
}
