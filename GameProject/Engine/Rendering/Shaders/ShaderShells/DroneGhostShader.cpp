#include "DroneGhostShader.h"
#include "Engine/AssetManagement/TextureManager.h"

DroneGhostShader::DroneGhostShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix)
	: EntityShader("./Engine/Rendering/Shaders/droneGhostShaderInstanced.vert", "./Engine/Rendering/Shaders/droneGhostShaderInstanced.frag", shadowBuffer, camera, lightSpaceMatrix)
{
	this->noiseTexture = TextureManager::loadTexture("./Game/assets/textures/noise.jpg");

	this->noiseTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	this->noiseTexture->unbind();
}

DroneGhostShader::~DroneGhostShader()
{
}

void DroneGhostShader::bind()
{
	EntityShader::bind();
	Shader::setTexture2D("noiseTex", 2, noiseTexture->getID());
	Shader::setUniform1f("time", EntityShader::getTime());
	//Shader::setUniformMatrix4fv("view", 1, false, &((*this->camera)->getView()[0][0]));
}

void DroneGhostShader::updateMeshData(unsigned texId)
{
	EntityShader::updateMeshData(texId);
}

void DroneGhostShader::update(const float & dt)
{
	EntityShader::update(dt);
}
