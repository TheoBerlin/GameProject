#include "ParticleManager.h"

void ParticleManager::updateEmitter(ParticleEmitter* emitter, float dt)
{
	emitter->update(dt);
}

ParticleManager & ParticleManager::get()
{
	static ParticleManager particleManager;
	return particleManager;
}

void ParticleManager::update(float dt)
{
	//int half = std::ceil((float)emitters.size() / 2.0f);

	//ParticleManager * p = new ParticleManager();

	//std::thread first(&ParticleManager::updateEmitter, p, emitters[0], dt);

	//first.join();

	//delete p;

	for (int i = 0; i < emitters.size(); i++) {
		emitters[i]->update(dt);
	}
}

void ParticleManager::addEmitter(ParticleEmitter* emitter)
{
	emitters.push_back(emitter);
}

int ParticleManager::getMaxParticles() const
{
	int maxParticle = 0;
	for (int i = 0; i < emitters.size(); i++) {
		maxParticle += emitters[i]->getMaxParticle();
	}
	return maxParticle;
}

int ParticleManager::getParticleCount() const
{
	int countParticle = 0;
	for (int i = 0; i < emitters.size(); i++) {
		countParticle += emitters[i]->getParticleArray().size();
	}
	return countParticle;
}

void ParticleManager::updateBuffer()
{
	int offset = 0;
	for (int i = 0; i < emitters.size(); i++) {
		if (emitters[i]->getParticleArray().size() > 0) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, emitters[i]->getParticleArray().size() * sizeof(Particle), &emitters[i]->getParticleArray().front());
			offset += emitters[i]->getParticleArray().size() * sizeof(Particle);
		}
	}
}