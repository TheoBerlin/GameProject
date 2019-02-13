#include "ParticleManager.h"

void ParticleManager::update(float dt, glm::vec3 velocity, float scale)
{
	for (int i = 0; i < emitters.size(); i++) {
		emitters[i].update(dt, velocity, scale);
	}
}

void ParticleManager::addEmitter(glm::vec3 position , glm::vec3 startVelocity, glm::vec3 startColour, float startScale, int maxParticle, int spawnRate, float spread, glm::vec3 endColour)
{
	ParticleEmitter emitter(position, startVelocity, startColour, startScale, maxParticle, spawnRate, spread, endColour);
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

void ParticleManager::updateBuffer()
{
	int offset = 0;
	for (int i = 0; i < emitters.size(); i++) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, emitters[i].getParticleArray().size() * sizeof(Particle), &emitters[i].getParticleArray().front());
		offset += emitters[i].getParticleArray().size() * sizeof(Particle);
	}
}

std::vector<Particle> ParticleManager::getParticleArray() const
{
	return emitters[0].getParticleArray();
}
