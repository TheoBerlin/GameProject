#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class RoofShader : public EntityShader
{
public:
	RoofShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	virtual ~RoofShader();

	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

private:
	Texture* wallTexture;
};

