#pragma once

#include "ParticleEmitter.h"
#include <vector>
#include <unordered_map>

class ParticleManager {
private:
	std::vector<ParticleEmitter> emitters;

public:
	void update(float dt);
	/*
	Adds an emitter to the world.
	Position is emitter starting position.
	Velocity is starting velocity of particles spawned by emitter.
	Gravity is velocity change of particles over time.
	maxParticle is how many particles will be spawned and spawn rate is how many is spawned per second. maxParticle / spawnRate gives particle lifeTime.
	Spread is how much particles can randomly divert from starting velocity direction.
	*/
	void addEmitter(glm::vec3 position, glm::vec3 velocity, glm::vec3 gravity, int maxParticle, int spawnRate, float spread);
	int getMaxParticles() const;
	int getParticleCount() const;
	std::vector<Particle*> getParticleArray() const;
};