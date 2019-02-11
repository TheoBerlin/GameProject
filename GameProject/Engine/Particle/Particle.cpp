#include "Particle.h"

Particle::Particle(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float scale)
{
	this->position = position;
	this->velocity = velocity;
	this->colour = colour;
	this->scale = scale;
}

void Particle::update(float dt, glm::vec3 velocity, float scale)
{
	position += this->velocity * dt;
	if (velocity != glm::vec3(0.0f)) {
		this->velocity += velocity * dt;
	}
	if (scale != 0.0f && scale != 1.0f) {
		scale -= 1.0f;
		scale * dt;
		scale += 1.0f;
		this->scale *= scale;
	}
}

void Particle::reset(glm::vec3 position, glm::vec3 velocity, glm::vec3 colour, float scale)
{
	this->position = position;
	this->velocity = velocity;
	this->colour = colour;
	this->scale = scale;
}
