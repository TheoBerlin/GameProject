#include "RewindShader.h"

#include "Engine/AssetManagement/TextureManager.h"

RewindShader::RewindShader() : PostProcessShader("./Engine/Rendering/Shaders/RewindShader.vert", "./Engine/Rendering/Shaders/RewindShader.frag")
{

}


RewindShader::~RewindShader()
{
}

void RewindShader::bind(Texture * texture)
{
	PostProcessShader::bind(texture);
}
