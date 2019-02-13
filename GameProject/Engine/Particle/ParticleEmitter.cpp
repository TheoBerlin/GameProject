#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(glm::vec3 position, glm::vec3 velocity, glm::vec3 startColour, float startScale, int maxParticle, int spawnRate, float spread, glm::vec3 endColour)
{
	this->position = position;
	this->startVelocity = velocity;
	this->maxParticle = maxParticle;
	this->spawnRate = spawnRate;
	this->spread = spread;
	this->startColour = startColour;
	this->endColour = endColour;
	this->startScale = startScale;
	this->spread = spread;

	particles.reserve(maxParticle);

	oldestParticle = 0;
	emissionTime = 0;
}

void ParticleEmitter::particleUpdate(unsigned int index, float dt, glm::vec3 velocity, float scale)
{
	particles[index].position += (particlesInfo[index].velocity + particlesInfo[index].spread) * dt;
	if (velocity != glm::vec3(0.0f)) {
		particlesInfo[index].velocity += velocity * dt;
	}
	particlesInfo[index].lifeTime -= dt;
	if (startColour != endColour) {
		interpolateColour(index);
	}
	if (scale != 0.0f && scale != 1.0f) {
		scale -= 1.0f;
		scale * dt;
		scale += 1.0f;
		particles[index].scale *= scale;
	}
}

void ParticleEmitter::particleReset(unsigned int index)
{
	particles[index].position = position;
	particles[index].colour = startColour;
	particles[index].scale = startScale;
	particlesInfo[index].velocity = startVelocity;
	particlesInfo[index].lifeTime = maxParticle / spawnRate;
	std::uniform_real_distribution<> r(-spread, spread);
	particlesInfo[index].spread = glm::vec3(r(ran), r(ran), r(ran));
}

void ParticleEmitter::interpolateColour(unsigned int index)
{
	float maxLife = maxParticle / spawnRate;
	particles[index].colour = startColour * (particlesInfo[index].lifeTime / maxLife) + endColour * (1 - particlesInfo[index].lifeTime / maxLife);;
}

void ParticleEmitter::update(float dt, glm::vec3 velocity, float scale)
{
	emissionTime += dt;
	while (emissionTime >= ((float)1 / spawnRate)) {
		emissionTime -= ((float)1 / spawnRate);
		if (particles.size() < maxParticle) {
			particles.push_back(Particle());
			particles[particles.size() - 1].position = position;
			particles[particles.size() - 1].colour = startColour;
			particles[particles.size() - 1].scale = startScale;
			particlesInfo.push_back(ParticleUpdateInfo());
			particlesInfo[particlesInfo.size() - 1].velocity = startVelocity;
			particlesInfo[particlesInfo.size() - 1].lifeTime = maxParticle/spawnRate;
			std::uniform_real_distribution<> r(-spread, spread);
			particlesInfo[particlesInfo.size() - 1].spread = glm::vec3(r(ran), r(ran), r(ran));
		}
		else {
			particleReset(oldestParticle++);
			if (oldestParticle == maxParticle)
				oldestParticle = 0;
		}
	}
	for (int i = 0; i < particles.size(); i++) {
		particleUpdate(i, dt, velocity, scale);
	}
}

std::vector<Particle> ParticleEmitter::getParticleArray() const
{
	return particles;
}

int ParticleEmitter::getMaxParticle() const
{
	return maxParticle;
}
