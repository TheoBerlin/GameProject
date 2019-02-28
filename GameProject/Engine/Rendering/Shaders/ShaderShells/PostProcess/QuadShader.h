#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/PostProcess/PostProcessShader.h"

class QuadShader : public PostProcessShader
{
public:
	QuadShader();
	virtual ~QuadShader();

	virtual void bind(Texture* texture);

};

