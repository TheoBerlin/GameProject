#pragma once

#include "Engine/Rendering/Shaders/ShaderShells/EntityShader.h"
#include "Engine/Rendering/GLAbstraction/Texture.h"

class InfinityPlanePrePassShader : public EntityShader
{
public:
	InfinityPlanePrePassShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix);
	~InfinityPlanePrePassShader();

	void bind();
	void updateMeshData(unsigned texId);
	void update(const float& dt);

};