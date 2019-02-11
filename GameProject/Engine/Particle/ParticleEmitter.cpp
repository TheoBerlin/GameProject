#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 velocity, glm::vec3 gravity, int maxParticle, int spawnRate, float spread)
{
	this->position = position;
	this->velocity = velocity;
	this->gravity = gravity;
	this->maxParticle = maxParticle;
	this->spawnRate = spawnRate;
	this->spread = spread;
}
