#include "PostProcessShader.h"

#include "Engine/Rendering/GLAbstraction/Texture.h"

float PostProcessShader::time = 0.0f;

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

void PostProcessShader::update(const float & dt)
{
	time += dt;
	time = fmod(time, 20000.0f);
}

float PostProcessShader::getTime()
{
	return time;
}
