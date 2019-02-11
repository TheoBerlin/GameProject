#pragma once

#include <glm/glm.hpp>

class ParticleEmitter {
private:
	struct Partice {
		glm::vec3 position;
		glm::vec3 colour;
		float scale;
		float timeAlive;
		float lifeTime;
	};
	struct quadData {
		float vertex[12] = {
			-0.5, -0.5, 0.0, //BOTTOM LEFT
			-0.5,  0.5, 0.0, //TOP LEFT
			 0.5,  0.5, 0.0, //TOP RIGHT
			 0.5, -0.5, 0.0 //BOTTOM RIGHT
		};
		int n[6] = {
			0, 1, 2,
			2, 3, 0
		};
	};

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 gravity;

	int maxParticle;
	int nrOfParticle;
	int spawnRate;
	float spread;
public:
	ParticleEmitter(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 velocity = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 gravity = glm::vec3(0.0f, 0.0f, 0.0f), int maxParticle = 100, int spawnRate = 10, float spread = 0.0f);

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