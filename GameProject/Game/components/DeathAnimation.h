#pragma once

#include <Engine/Components/Component.h>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>

class Entity;

class DeathAnimation : public Component
{
public:
	DeathAnimation(Entity* host);
	~DeathAnimation();

	void update(const float& dt);

	void play(float totalTime = 1.3f);

	void reset();

private:
	void shake();
	void fall();

	float totalTime;
	float currentTime;

	// Fall
	glm::vec3 pushDir;
	float pushSpeed;
	float gravity;
	float totalAngle;

	const glm::vec3 maxFallTranslation = {0.0, -1.5f, 0.0f};
	glm::vec3 totalFallTranslation, totalFallRotation;
	const glm::vec3 maxFallRotation = {0.0f, glm::half_pi<float>(), 0.0f};

	// Shake
	const float shakeTime = 0.5f;

	const glm::vec3 maxShakeTranslation = {0.0f, 0.2f, 0.0f};
	glm::vec3 totalShakeTranslation, totalShakeRotation;
	const glm::vec3 maxShakeRotation = { 0.f, 1.0f, 1.0f};
};
