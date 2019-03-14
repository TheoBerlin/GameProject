#include "RoofShader.h"

#include "../../../AssetManagement/TextureManager.h"

RoofShader::RoofShader(Camera ** camera)
	: EntityShader("./Engine/Rendering/Shaders/RoofShader.vert", "./Engine/Rendering/Shaders/RoofShader.frag", nullptr, camera, nullptr)
{
	this->wallTexture = TextureManager::loadTexture("./Game/assets/textures/noise.jpg");

	this->wallTexture->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	this->wallTexture->unbind();
}

RoofShader::~RoofShader()
{
}

void RoofShader::bind()
{
	EntityShader::bind();
}

void RoofShader::updateMeshData(unsigned texId)
{
	EntityShader::updateMeshData(texId);
}

void RoofShader::update(const float & dt)
{
	EntityShader::update(dt);
}
