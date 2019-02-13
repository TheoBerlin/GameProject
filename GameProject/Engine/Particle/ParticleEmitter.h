#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include "Engine/Config.h"

//Values which shader is intrested in
struct Particle {
	glm::vec3 position;
	float scale;
	glm::vec3 colour;
};

class ParticleEmitter {
private:
	//Values to change particle structs values for every particle.
	//Structs are seperated to not buffer this data for shader
	struct ParticleUpdateInfo {
		glm::vec3 velocity;
		float lifeTime;
		glm::vec3 spread = glm::vec3(0.0f);
	};

	glm::vec3 position;
	glm::vec3 startVelocity;
	glm::vec3 startColour;
	glm::vec3 endColour;
	float startScale;

	int maxParticle;
	int oldestParticle;
	int spawnRate;
	float spread;

	float emissionTime;

	//Particles
	std::vector<Particle> particles;
	std::vector<ParticleUpdateInfo> particlesInfo;
	void particleUpdate(unsigned int index, float dt, glm::vec3 velocity, float scale);
	void particleReset(unsigned int index);
	void interpolateColour(unsigned int index);
public:
	ParticleEmitter(glm::vec3 position, glm::vec3 startVelocity, glm::vec3 startColour, float startScale, int maxParticle, int spawnRate, float spread, glm::vec3 endColour);

	void update(float dt, glm::vec3 velocity, float scale);

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