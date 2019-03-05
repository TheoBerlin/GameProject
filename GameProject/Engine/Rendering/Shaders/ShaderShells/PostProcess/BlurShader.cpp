#include "BlurShader.h"



BlurShader::BlurShader() : PostProcessShader("./Engine/Rendering/Shaders/BlurFilter.vert", "./Engine/Rendering/Shaders/BlurFilter.frag")
{
}


BlurShader::~BlurShader()
{
}

void BlurShader::bind(Texture * texture, bool horizontal)
{
	PostProcessShader::bind(texture);
	this->setUniform1b("horizontal", horizontal);
}
