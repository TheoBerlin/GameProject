#pragma once

#include <glm/glm.hpp>

class Particle {
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 colour;
	float scale;

public:
	Particle(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float scale);

	void update(float dt, glm::vec3 velocity, float scale);
	void reset(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float scale);
};