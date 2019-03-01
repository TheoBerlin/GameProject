#include "PostProcessShader.h"

#include "Engine/Rendering/GLAbstraction/Texture.h"

PostProcessShader::PostProcessShader(const std::string& vertex, const std::string& fragment) : Shader(vertex, fragment)
{
}


PostProcessShader::~PostProcessShader()
{
}

void PostProcessShader::bind(Texture * texture)
{
	Shader::bind();
	this->setTexture2D("tex", 0, texture->getID());
}
