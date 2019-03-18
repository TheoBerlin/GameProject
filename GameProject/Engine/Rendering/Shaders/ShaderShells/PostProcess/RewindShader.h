#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/PostProcess/PostProcessShader.h"

class Texture;

class RewindShader : public PostProcessShader
{
public:
	RewindShader();
	~RewindShader();

	virtual void bind(Texture* texture);

private:
};

