#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include "Engine/Config.h"

//Values which shader is intrested in
struct Particle {
	glm::vec3 position;
	float scale;
	glm::vec4 colour;
};

class ParticleEmitter {
private:
	//Values to change particle structs values for every particle.
	//Structs are seperated to not buffer this data for shader
	struct ParticleUpdateInfo {
		glm::vec3 velocity = glm::vec3(1.f);
		float life = 0;
		glm::vec3 spread = glm::vec3(0.0f);
	};

	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 startVelocity;
	glm::vec3 acceleration;
	glm::vec4 startColour;
	glm::vec4 endColour;
	float startScale;
	float scaleChange;

	int maxParticle;
	int oldestParticle;
	int spawnRate;
	float spread;
	float lifeTime;

	float spawnTime;
	float duration;
	bool loop;
	bool allParticlesDead;

	// Position in particle managers vector
	size_t emitterVectorIndex;

	//Particles
	std::vector<Particle> particles;
	std::vector<ParticleUpdateInfo> particlesInfo;
	void particleUpdate(unsigned int index, float dt, glm::vec3 velocity, float scale);
	void particleReset(unsigned int index);
	void interpolateColour(unsigned int index);
public:
	ParticleEmitter();
	void operator=(ParticleEmitter &oldEmitter);

	void update(float dt);
	//Send how long the emitter should play, give 0 duration to play indefinitely.
	void playEmitter(float duration);
	//Stop emitter
	void stopEmitter();

	//Clear all particles
	void clearEmitter();

	std::vector<Particle> getParticleArray() const;

	//Set emitter position
	void setPosition(const glm::vec3 position);
	//Get emitter position
	glm::vec3 getPosition() const;
	//Set the velocity particles exit from emitter
	void setVelocity(const glm::vec3 velocity);
	//Get the velocity particles exit from emitter
	glm::vec3 getVelocity() const;
	//Set how fast particles accelerate per second
	void setAcceleration(const glm::vec3 acceleration);
	//Get how fast particles accelarate per second
	glm::vec3 getAcceleration() const;
	//Set the start scale of particles
	void setScale(const float scale);
	//get the start scale of particles
	float getScale() const;
	//Set how fast particles scale per second
	void setScaleChange(const float scaleChange);
	//Get how fast particles scale per second
	float getScaleChange() const;
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
	float getSpread() const;
	//Set particles life time, if looping this should be maximum particles divided by spawnRate
	void setLifeTime(const float lifeTime);
	//Get particles life time
	float getLifeTime() const;
	//Set the Colour+Transperency which particles start with. Will also set end colour to same. call setEndColour after this to set colour change over time
	void setStartColour(const glm::vec4 startColour);
	//Get start colour of particles
	glm::vec4 getStartColour() const;
	//Set the Colour+Transperency that particles will have at the end of their life. This should be called after setStartColour since that function overrides endColour
	void setEndColour(const glm::vec4 endColour);
	//Get particles end colour
	glm::vec4 getEndColour() const;

	/*
		Is true when no particles in the emitter lives
	*/
	bool isDead();

	/*
		Used for removing emitters in particle manager
	*/
	size_t getEmitterVectorIndex() const;
	void setEmitterVectorIndex(const size_t& index);

};