#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 velocity, glm::vec3 gravity, int maxParticle, int spawnRate, float spread)
{
	this->position = position;
	this->velocity = velocity;
	this->gravity = gravity;
	this->maxParticle = maxParticle;
	this->spawnRate = spawnRate;
	this->spread = spread;

	oldestParticle = 0;
	emissionTime = 0;
}

void ParticleEmitter::update(float dt)
{
	emissionTime += dt;
	while (emissionTime >= 1 / spawnRate) {
		emissionTime -= 1 / spawnRate;
		if (particles.size() != maxParticle) {
			Particle particle(position, velocity, glm::vec3(1.0f), 1.0f);
			particles.push_back(particle);
		} else {
			particles[oldestParticle++].reset(position, velocity, glm::vec3(1.0f), 1.0f);
		}
	}
	for (int i = 0; i < particles.size(); i++) {
		particles[i].update(dt, glm::vec3(0.0f), 1.0f);
	}
}

std::vector<Particle> ParticleEmitter::getParticleArray() const
{
	return particles;
}
