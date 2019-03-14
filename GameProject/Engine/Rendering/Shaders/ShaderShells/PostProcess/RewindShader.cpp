#include "RewindShader.h"

#include "Engine/AssetManagement/TextureManager.h"

RewindShader::RewindShader() : PostProcessShader("./Engine/Rendering/Shaders/RewindShader.vert", "./Engine/Rendering/Shaders/RewindShader.frag")
{
	this->rewindTexture = TextureManager::loadTexture("./Game/assets/textures/RewindEffect.png");
}


RewindShader::~RewindShader()
{
}

void RewindShader::bind(Texture * texture)
{
	PostProcessShader::bind(texture);

	Shader::setTexture2D("rewindTex", 1, this->rewindTexture->getID());
}
