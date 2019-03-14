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

	void play(float totalTime = 0.4f, float gravity = -0.9f, float totalAngle = glm::pi<float>()*.5f, const glm::vec3& pushDir = glm::vec3(0.f, -1.f, 0.f), float pushSpeed = 0.5f);

	void reset();

private:
	bool shake(float dt);
	void fall(float dt);

	float totalTime;
	float currentTime;

	glm::vec3 pushDir;
	float pushSpeed;
	float gravity;
	float totalAngle;

	// Spasm
	glm::vec3 maxTranslation = { 0.f, 0.2f, 0.f };
	const glm::vec3 maxRotation = { 0.f, glm::quarter_pi<float>() / 4.0f, glm::quarter_pi<float>() / 4.0f };
	float shakeTimer;
};