#pragma once

#include <Engine/Components/Component.h>
#include <glm/vec3.hpp>

class Entity;
class DeathAnimation : public Component
{
public:
	DeathAnimation(Entity* host);
	~DeathAnimation();

	void update(const float& dt);

	void play(const glm::vec3& dir = glm::vec3(0.f, -1.f, 0.f), float speed = 0.5f);

	void reset();

private:
	float totalTime;
	glm::vec3 dir;
	float speed;
};