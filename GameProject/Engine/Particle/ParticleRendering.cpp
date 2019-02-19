#include "ParticleRendering.h"

ParticleRendering::ParticleRendering()
{
}

void ParticleRendering::init(std::string vertexShader, std::string fragmentShader)
{
	this->particleShader = new Shader("./Engine/Particle/Particle.vert", "./Engine/Particle/Particle.frag");
}

void ParticleRendering::bind()
{
	this->particleShader->bind();
}

void ParticleRendering::unbind()
{
	this->particleShader->unbind();
}
