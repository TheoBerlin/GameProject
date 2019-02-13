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

ParticleEmitter::~ParticleEmitter()
{
	for (int i = 0; i < particles.size(); i++) {
		delete particles[i];
	}
}

void ParticleEmitter::particleUpdate(unsigned int index, float dt, glm::vec3 velocity, float scale)
{
	for (int i = 0; i < 3; i++) {
		particles[index]->position[i] += particles[index]->velocity[i] * dt;
		if (velocity != glm::vec3(0.0f)) {
			particles[index]->velocity[i] += velocity[i] * dt;
		}
	}
	if (scale != 0.0f && scale != 1.0f) {
		scale -= 1.0f;
		scale * dt;
		scale += 1.0f;
		particles[index]->scale *= scale;
	}
}

void ParticleEmitter::particleReset(unsigned int index, glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float scale)
{
	for (int i = 0; i < 3; i++) {
		particles[index]->position[i] = position[i];
		particles[index]->velocity[i] = velocity[i];
		particles[index]->colour[i] = colour[i];
	}
	particles[index]->scale = scale;
}

void ParticleEmitter::update(float dt)
{
	emissionTime += dt;
	while (emissionTime >= ((float)1 / spawnRate)) {
		emissionTime -= ((float)1 / spawnRate);
		if (particles.size() < maxParticle) {
			Particle* particle = new Particle;
			for (int i = 0; i < 3; i++) {
				particle->position[i] = position[i];
				particle->velocity[i] = velocity[i];
				particle->colour[i] = 1.0f;
			}
			particle->scale = 0.5f;
			particles.push_back(particle);
		} else {
			particleReset(oldestParticle++, position, velocity, glm::vec3(1.0f), 1.0f);
			if (oldestParticle == maxParticle)
				oldestParticle = 0;
		}
	}
	for (int i = 0; i < particles.size(); i++) {
		particleUpdate(i, dt, glm::vec3(0.0f), 1.0f);
	}
}

std::vector<Particle*> ParticleEmitter::getParticleArray() const
{
	return particles;
}

int ParticleEmitter::getMaxParticle() const
{
	return maxParticle;
}
