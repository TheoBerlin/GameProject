#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class InfinityPlaneShader : public EntityShader
{
public:
	InfinityPlaneShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	~InfinityPlaneShader();

	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

private:
	Texture* wallTexture;
};

