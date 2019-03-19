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
			shake();
		}

		else if (currentTime < totalTime) {
			fall();
		}

		else {
			host->getPausedTransform()->translate({ 0.f, -100.f, 0.f });
			this->currentTime = this->totalTime;
		}
	}
}

void DeathAnimation::play(float totalTime)
{
	this->host->pauseModelTransform();

	this->totalTime = totalTime;
	this->currentTime = 0.f;
	this->pushDir = pushDir;
	this->pushSpeed = pushSpeed;
	this->gravity = gravity;
	this->totalShakeTranslation = {0.0f, 0.0f, 0.0f};
	this->totalShakeRotation = {0.0f, 0.0f, 0.0f};
	this->totalFallTranslation = {0.0f, 0.0f, 0.0f};
	this->totalFallRotation = {0.0f, 0.0f, 0.0f};
}

void DeathAnimation::reset()
{
	this->host->unpauseModelTransform();
	this->currentTime = 0.0f;
	this->totalTime = 0.0f;
	this->totalShakeTranslation = {0.0f, 0.0f, 0.0f};
	this->totalShakeRotation = {0.0f, 0.0f, 0.0f};
	this->totalFallTranslation = {0.0f, 0.0f, 0.0f};
	this->totalFallRotation = {0.0f, 0.0f, 0.0f};
}

void DeathAnimation::shake()
{
	// Translate
	float translationFactor = Utils::map(0.f, shakeTime*shakeTime, currentTime * currentTime, 0.0f, 1.0f);

	glm::vec3 translation = translationFactor * maxShakeTranslation;

	// Apply transformations
	Transform* transform = this->host->getPausedTransform();

	transform->translate(translation - totalShakeTranslation);

	totalShakeTranslation = translation;

	float frequency = Utils::map(0.f, shakeTime, currentTime, 20.f, 40.f);
	float amplitude = Utils::map(0.f, shakeTime, currentTime, 0.f, glm::half_pi<float>() / 8.0f);

	glm::vec3 rotation;

	rotation.x = cosf(currentTime * frequency) * amplitude;
	rotation.y = 0.0f;
	rotation.z = sinf(currentTime * frequency) * amplitude;

	transform->rotate(rotation - totalShakeRotation);

	totalShakeRotation = rotation;
}

void DeathAnimation::fall()
{
	Transform* transform = this->host->getPausedTransform();

	float animationFactor = Utils::map(this->shakeTime, this->totalTime, currentTime, 0.0f, 1.0f);

	// Translate
	glm::vec3 translation = maxFallTranslation * animationFactor;

	transform->translate(translation - totalFallTranslation);

	totalFallTranslation = translation;

	// Rotate
	glm::vec3 rotation = maxFallRotation * animationFactor;

	transform->rotate(rotation.x - totalFallRotation.x, rotation.y - totalFallRotation.y, rotation.z - totalFallRotation.z);

	totalFallRotation = rotation;
}
