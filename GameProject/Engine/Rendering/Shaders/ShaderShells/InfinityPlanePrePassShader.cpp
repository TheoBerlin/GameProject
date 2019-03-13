#include "InfinityPlanePrePassShader.h"


InfinityPlanePrePassShader::InfinityPlanePrePassShader(Framebuffer * shadowBuffer, Camera ** camera, glm::mat4 * lightSpaceMatrix)
	: EntityShader("./Engine/Rendering/Shaders/InfinityPlanePrePassShader.vert", "./Engine/Rendering/Shaders/InfinityPlanePrePassShader.frag", shadowBuffer, camera, lightSpaceMatrix)
{

}

InfinityPlanePrePassShader::~InfinityPlanePrePassShader()
{
}

void InfinityPlanePrePassShader::bind()
{
	Shader::bind();
	EntityShader::setCameraUniform();
}

void InfinityPlanePrePassShader::updateMeshData(unsigned texId)
{
	EntityShader::updateMeshData(texId);
}

void InfinityPlanePrePassShader::update(const float & dt)
{
	EntityShader::update(dt);
}
