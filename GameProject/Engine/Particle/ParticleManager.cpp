#include "ParticleManager.h"

void ParticleManager::update(float dt)
{
	for (int i = 0; i < emitters.size(); i++) {
		emitters[i].update(dt);
	}
}

void ParticleManager::addEmitter(glm::vec3 position, glm::vec3 velocity, glm::vec3 gravity, int maxParticle, int spawnRate, float spread)
{
	ParticleEmitter emitter(position, velocity, gravity, maxParticle, spawnRate, spread);
	emitters.push_back(emitter);
}

int ParticleManager::getMaxParticles() const
{
	int maxParticle = 0;
	for (int i = 0; i < emitters.size(); i++) {
		maxParticle += emitters[i].getMaxParticle();
	}
	return maxParticle;
}

int ParticleManager::getParticleCount() const
{
	int countParticle = 0;
	for (int i = 0; i < emitters.size(); i++) {
		countParticle += emitters[i].getParticleArray().size();
	}
	return countParticle;
}

std::vector<Particle*> ParticleManager::getParticleArray() const
{
	return emitters[0].getParticleArray();
}
