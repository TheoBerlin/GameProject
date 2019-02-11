#pragma once

#include "ParticleEmitter.h"
#include <vector>
#include <unordered_map>

class ParticleManager {
private:
	static const GLfloat vertexData[] = {
	-0.5, -0.5, 0.0, //BOTTOM LEFT
	-0.5,  0.5, 0.0, //TOP LEFT
	 0.5,  0.5, 0.0, //TOP RIGHT
	 0.5, -0.5, 0.0 //BOTTOM RIGHT
	};

	std::vector<ParticleEmitter> emitters;
	std::vector<std::vector<Particle>> particlesArray;
	int nrOfEmitters;



public:
	void update(float dt);
	void draw();

	void addEmitter(glm::vec3 position, glm::vec3 velocity, glm::vec3 gravity, int maxParticle, int spawnRate, float spread);

};