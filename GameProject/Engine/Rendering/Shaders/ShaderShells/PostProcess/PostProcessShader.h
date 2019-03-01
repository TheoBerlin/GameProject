#pragma once

#include "Engine/Rendering/GLAbstraction/Shader.h"

class PostProcessShader : public Shader
{
public:
	PostProcessShader(const std::string& vertex, const std::string& fragment);
	~PostProcessShader();

	virtual void bind(Texture* texture);
};
