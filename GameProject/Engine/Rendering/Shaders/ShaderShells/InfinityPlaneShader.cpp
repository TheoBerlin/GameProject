#include "InfinityPlaneShader.h"

#include "Engine/AssetManagement/TextureManager.h"

InfinityPlaneShader::InfinityPlaneShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix)
	: EntityShader("./Engine/Rendering/Shaders/InfinityPlaneShader.vert", "./Engine/Rendering/Shaders/InfinityPlaneShader.frag", shadowBuffer, camera, lightSpaceMatrix)
{
	this->wallTexture = TextureManager::loadTexture("./Game/assets/textures/noise.jpg");

	this->wallTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	this->wallTexture->unbind();


}

InfinityPlaneShader::~InfinityPlaneShader()
{
}

void InfinityPlaneShader::bind()
{
	EntityShader::bind();
}

void InfinityPlaneShader::updateMeshData(unsigned texId)
{
	EntityShader::updateMeshData(texId);
}

void InfinityPlaneShader::update(const float & dt)
{
	EntityShader::update(dt);
}
