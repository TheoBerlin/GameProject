#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class WallShader : public EntityShader
{
public:
	WallShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	~WallShader();


	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

private:
	Texture* wallTexture;
};

