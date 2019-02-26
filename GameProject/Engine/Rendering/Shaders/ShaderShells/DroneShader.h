#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class DroneShader : public EntityShader
{
public:
	DroneShader(const std::string & vertex, const std::string & fragment, Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	~DroneShader();


	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

private:
	Texture* noiseTexture;
};

