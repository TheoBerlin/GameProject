#include "WallShader.h"

#include "Engine/AssetManagement/TextureManager.h"

WallShader::WallShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix)
	: EntityShader("./Engine/Rendering/Shaders/wallShaderInstanced.vert", "./Engine/Rendering/Shaders/wallShaderInstanced.frag", shadowBuffer, camera, lightSpaceMatrix)
{
	this->wallTexture = TextureManager::loadTexture("./Game/assets/textures/noise.jpg");

	this->wallTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	this->wallTexture->unbind();
}

WallShader::~WallShader()
{
}

void WallShader::bind()
{
	EntityShader::bind();
}

void WallShader::updateMeshData(unsigned texId)
{
	EntityShader::updateMeshData(texId);
}

void WallShader::update(const float & dt)
{
	EntityShader::update(dt);
}
