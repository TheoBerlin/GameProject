#pragma once

#include "GLAbstraction/RenderingResources.h"
#include "GLAbstraction/Framebuffer.h"
#include "GLAbstraction/Shader.h"


class Pipeline
{
public:
	Pipeline();
	~Pipeline();

	Texture* drawToTexture();
	void drawTextureToQuad(const Texture& tex);

private:
	Framebuffer fbo;

};

