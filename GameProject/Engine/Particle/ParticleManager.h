#pragma once

#include "ParticleEmitter.h"
#include <vector>
#include <unordered_map>
#include "GL/glew.h"

class ParticleManager {
private:
	std::vector<ParticleEmitter> emitters;

public:
	void update(float dt, glm::vec3 velocity, float scale);
	/*
	Adds an emitter to the world.
	Position is emitter starting position.
	Velocity is starting velocity of particles spawned by emitter.
	Gravity is velocity change of particles over time.
	maxParticle is how many particles will be spawned and spawn rate is how many is spawned per second. maxParticle / spawnRate gives particle lifeTime.
	Spread is how much particles can randomly divert from starting velocity direction.
	*/
	void addEmitter(glm::vec3 position = glm::vec3(0.0f), glm::vec3 startVelocity = glm::vec3(0.0f), glm::vec3 startColour = glm::vec3(1.0f), float startScale = 1.0f, int maxParticle = 100, int spawnRate = 10, float spread = 0.0f, glm::vec3 endColour = glm::vec3(1.0f));
	int getMaxParticles() const;
	int getParticleCount() const;
	void updateBuffer();
	std::vector<Particle> getParticleArray() const;
};