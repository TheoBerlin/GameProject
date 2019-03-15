#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class DroneGhostShader : public EntityShader
{
public:
	DroneGhostShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	~DroneGhostShader();

	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

private:
	Texture* noiseTexture;
};

