#pragma once

#include <glm/glm.hpp>
#include "Particle.h"
#include <vector>

class ParticleEmitter {
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 gravity;

	int maxParticle;
	int oldestParticle;
	int spawnRate;
	float spread;

	float emissionTime;
	std::vector<Particle> particles;
public:
	ParticleEmitter(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 velocity = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 gravity = glm::vec3(0.0f, 0.0f, 0.0f), int maxParticle = 100, int spawnRate = 10, float spread = 0.0f);

	void update(float dt);

	std::vector<Particle> getParticleArray() const;

	//Set emitter position
	void setPosition(const glm::vec3 position);
	//Get emitter position
	glm::vec3 getPosition() const;
	//Set the velocity particles exit from emitter
	void setVelocity(const glm::vec3 forward);
	//Get the velocity particles exit from emitter
	glm::vec3 getVelocity() const;
	//Set gravity strengt and direction for particles. Set this to (0, 0, 0) to disable gravity
	void setGravity(const glm::vec3 gravity);
	//Get gravity
	glm::vec3 getGravity() const;
	//Set maximum amount of particles
	void setMaxParticle(const int maxParticle);
	//Get maximum amount of particles
	int getMaxParticle() const;
	//Set amount of particles created a second
	void setSpawnRate(const int spawnRate);
	//Get amount of particles spawned a second
	int getSpawnRate() const;
	//Set how much particles should spread from center
	void setSpread(const float spread);
	//Get how much particles spread
	int getSpread() const;

};