#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
{
	this->position = glm::vec3(0.0f);
	this->prevPosition = glm::vec3(0.0f);
	this->startVelocity = glm::vec3(0.0f);
	this->maxParticle = 100;
	this->spawnRate = 10;
	this->spread = 0.0f;
	this->startColour = glm::vec4(1.0f);
	this->endColour = glm::vec4(1.0f);
	this->startScale = 1.0f;
	this->lifeTime = 10.0f;

	particles.reserve(maxParticle);

	loop = false;
	duration = 0;
	oldestParticle = 0;
	emissionTime = 0;
}

void ParticleEmitter::operator=(ParticleEmitter & oldEmitter)
{
	this->position = oldEmitter.getPosition();
	this->startVelocity = oldEmitter.getVelocity();
	this->maxParticle = oldEmitter.getMaxParticle();;
	this->spawnRate = oldEmitter.getSpawnRate();
	this->spread = oldEmitter.getSpread();
	this->startColour = oldEmitter.getStartColour();;
	this->endColour = oldEmitter.getEndColour();
	this->startScale = oldEmitter.getScale();
	this->lifeTime = oldEmitter.getLifeTime();
	this->acceleration = oldEmitter.getAcceleration();
	this->scaleChange = oldEmitter.getScaleChange();

	particles.reserve(maxParticle);

	loop = false;
	duration = 0;
	oldestParticle = 0;
	emissionTime = 0;
}

void ParticleEmitter::particleUpdate(unsigned int index, float dt, glm::vec3 acceleration, float scale)
{
	//Update position
	particles[index].position += (particlesInfo[index].velocity + particlesInfo[index].spread) * dt;
	if (acceleration != glm::vec3(0.0f)) {
		//update speed
		particlesInfo[index].velocity += acceleration * dt;
	}
	particlesInfo[index].life -= dt;
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
	particlesInfo[index].life = lifeTime;
	if (spread != 0) {
		std::uniform_real_distribution<> r(-spread, spread);
		particlesInfo[index].spread = glm::normalize(glm::vec3(r(ran), r(ran), r(ran))) * spread;
	}
}

void ParticleEmitter::interpolateColour(unsigned int index)
{
	particles[index].colour = startColour * (particlesInfo[index].life / lifeTime) + endColour * (1 - particlesInfo[index].life / lifeTime);;
}

void ParticleEmitter::update(float dt)
{
	//If not looping reduce duration
	if (!loop)
		duration -= dt;

	if (loop || duration > 0) {
		emissionTime += dt;
		int totalSpawn = 0;

		//Since logic update is caped, we can spawn multiple particals per frame
		//Keep spawning particles to accieve appropiate spawn rate
		while (emissionTime >= ((float)1 / spawnRate)) {
			totalSpawn++;
			emissionTime -= ((float)1 / spawnRate);
		}

		glm::vec3 dif = position - prevPosition;

		for (int i = 0; i < totalSpawn; i++) {
			//If all particles have not been spawned create a new one, else reset the oldest particle
			if (particles.size() < maxParticle) {
				//Create a new particle
				particles.push_back(Particle());
				particles[particles.size() - 1].position = position - (dif * ((float)(i + 1) / (float)totalSpawn));
				particles[particles.size() - 1].colour = startColour;
				particles[particles.size() - 1].scale = startScale;
				particlesInfo.push_back(ParticleUpdateInfo());
				particlesInfo[particlesInfo.size() - 1].velocity = startVelocity;
				particlesInfo[particlesInfo.size() - 1].life = lifeTime;
				if (spread != 0) {
					std::uniform_real_distribution<> r(-spread, spread);
					particlesInfo[particlesInfo.size() - 1].spread = glm::normalize(glm::vec3(r(ran), r(ran), r(ran))) * spread;
				}
			}
			else {
				particleReset(oldestParticle++);
				if (oldestParticle == maxParticle)
					oldestParticle = 0;
			}
		}
	}
	//Update all particles
	for (int i = 0; i < particles.size(); i++) {
		//Particle is dead if life is 0 or less
		if (particlesInfo[i].life > 0) {
			particleUpdate(i, dt, acceleration, scaleChange);
		}
		else {
			particles[i].scale = 0;
		}
	}
}

void ParticleEmitter::playEmitter(float duration)
{
	if (duration == 0) {
		loop = true;
	}
	else {
		this->duration = duration;
	}
}

void ParticleEmitter::stopEmitter()
{
	loop = false;
	duration = 0;
}

void ParticleEmitter::clearEmitter()
{
	particles.clear();
	oldestParticle = 0;
}

std::vector<Particle> ParticleEmitter::getParticleArray() const
{
	return particles;
}

void ParticleEmitter::setPosition(const glm::vec3 position)
{
	this->prevPosition = this->position;
	this->position = position;
}

glm::vec3 ParticleEmitter::getPosition() const
{
	return position;
}

void ParticleEmitter::setVelocity(const glm::vec3 velocity)
{
	this->startVelocity = velocity;
}

glm::vec3 ParticleEmitter::getVelocity() const
{
	return startVelocity;
}

void ParticleEmitter::setAcceleration(const glm::vec3 acceleration)
{
	this->acceleration = acceleration;
}

glm::vec3 ParticleEmitter::getAcceleration() const
{
	return acceleration;
}

void ParticleEmitter::setScale(float scale)
{
	this->startScale = scale;
}

float ParticleEmitter::getScale() const
{
	return startScale;
}

void ParticleEmitter::setScaleChange(float scaleChange)
{
	this->scaleChange = scaleChange;
}

float ParticleEmitter::getScaleChange() const
{
	return scaleChange;
}

void ParticleEmitter::setMaxParticle(const int maxParticle)
{
	this->maxParticle = maxParticle;
	particles.reserve(maxParticle);
}

int ParticleEmitter::getMaxParticle() const
{
	return maxParticle;
}

void ParticleEmitter::setSpawnRate(const int spawnRate)
{
	this->spawnRate = spawnRate;
}

int ParticleEmitter::getSpawnRate() const
{
	return spawnRate;
}

void ParticleEmitter::setSpread(const float spread)
{
	this->spread = spread;
}

int ParticleEmitter::getSpread() const
{
	return spread;
}

void ParticleEmitter::setLifeTime(const float lifeTime)
{
	this->lifeTime = lifeTime;
}

float ParticleEmitter::getLifeTime() const
{
	return lifeTime;
}

void ParticleEmitter::setStartColour(const glm::vec4 startColour)
{
	this->startColour = startColour;
	this->endColour = startColour;
}

glm::vec4 ParticleEmitter::getStartColour() const
{
	return startColour;
}

void ParticleEmitter::setEndColour(const glm::vec4 endColour)
{
	this->endColour = endColour;
}

glm::vec4 ParticleEmitter::getEndColour() const
{
	return endColour;
}
