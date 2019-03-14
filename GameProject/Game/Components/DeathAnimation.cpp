#include "DeathAnimation.h"

#include <Engine/Entity/Entity.h>
#include <Utils/Utils.h>

DeathAnimation::DeathAnimation(Entity * host) : Component(host, "DeathAnimation")
{
	this->totalTime = 0.f;
}

DeathAnimation::~DeathAnimation()
{
}

void DeathAnimation::update(const float & dt)
{
	if (this->currentTime < this->totalTime) {
		this->currentTime += dt;

		if (currentTime < shakeTime) {
			shake(dt);
		}

		else if (currentTime < totalTime) {
			fall(dt);
		}

		else if (currentTime > totalTime) {
			host->getPausedTransform()->translate({ 0.f, -100.f, 0.f });
			this->currentTime = this->totalTime;
		}
	}
}

void DeathAnimation::play(float totalTime, float gravity, float totalAngle, const glm::vec3& pushDir, float pushSpeed)
{
	this->host->pauseModelTransform();

	this->totalTime = totalTime;
	this->currentTime = 0.f;
	this->pushDir = pushDir;
	this->pushSpeed = pushSpeed;
	this->gravity = gravity;
	this->totalAngle = totalAngle;
	this->totalTranslation = {0.0f, 0.0f, 0.0f};
	this->translationFactor = 0.0f;
}

void DeathAnimation::reset()
{
	this->host->unpauseModelTransform();
	this->currentTime = 0.f;
}

void DeathAnimation::shake(float dt)
{
	// Translate
	float translationFactor = Utils::map(0.f, shakeTime*shakeTime, currentTime * currentTime, 0.0f, 1.0f);

	glm::vec3 translation = translationFactor * maxTranslation;

	// Apply transformations
	Transform* transform = this->host->getPausedTransform();

	transform->translate(translation - totalTranslation);

	totalTranslation = translation;

	float frequency = Utils::map(0.f, shakeTime, currentTime, 30.f, 60.f);
	float amplitude = Utils::map(0.f, shakeTime, currentTime, 3.f, 5.f);

	float x = cos(currentTime*frequency)*amplitude*dt;
	float z = sin(currentTime*frequency)*amplitude*dt;

	transform->rotate(x, 0.f, z);
}

void DeathAnimation::fall(float dt)
{
	const glm::vec3 gravityVec = { 0.f, this->gravity, 0.f };

	Transform* transform = this->host->getPausedTransform();
	transform->translate(this->pushDir*dt*this->pushSpeed + dt * gravityVec);

	transform->rotate(0.f, this->totalAngle*dt, 0.f);
}
