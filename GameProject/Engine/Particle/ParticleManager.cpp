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
	particlesArray.push_back(emitter.getParticleArray());
}
