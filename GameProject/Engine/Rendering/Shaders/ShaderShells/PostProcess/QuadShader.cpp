#include "QuadShader.h"

#include "Engine/Rendering/GLAbstraction/Texture.h"

QuadShader::QuadShader() 
	: PostProcessShader("./Engine/Rendering/Shaders/PostProcessVert.vert", "./Engine/Rendering/Shaders/PostProcessFrag.frag")
{
}

QuadShader::~QuadShader()
{
}

void QuadShader::bind(Texture* texture)
{
	PostProcessShader::bind(texture);
}
