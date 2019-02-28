#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/PostProcess/PostProcessShader.h"

class BlurShader : public PostProcessShader
{
public:
	BlurShader();
	virtual ~BlurShader();

	virtual void bind(Texture* texture, bool horizontal);
};

