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
		if (shake(dt)) {
			fall(dt);
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
}

void DeathAnimation::reset()
{
	this->host->unpauseModelTransform();
	this->shakeTimer = 0.f;
	this->currentTime = 0.f;
}

bool DeathAnimation::shake(float dt)
{
	shakeTimer += dt;
	const float shakeTime = 0.5f;

	if (shakeTimer <= shakeTime) {
		//spasmTimer = std::fmod(spasmTimer, glm::two_pi<float>());

		// Translate
		float translationFactor = Utils::map(0.f, shakeTime*shakeTime, shakeTimer*shakeTimer, 0.05f, 0.2f);
		glm::vec3 translation = translationFactor * glm::vec3(0.f, 0.1f, 0.f);

		// Apply transformations
		Transform* transform = this->host->getPausedTransform();

		transform->translate(translation);

		float frequency = Utils::map(0.f, shakeTime, shakeTimer, 30.f, 60.f);
		float amplitude = Utils::map(0.f, shakeTime, shakeTimer, 3.f, 5.f);
		
		float x = cos(shakeTimer*frequency)*amplitude*dt;
		float z = sin(shakeTimer*frequency)*amplitude*dt;
		transform->rotate(x, 0.f, z);
		return false;
	}

	return true;
}

void DeathAnimation::fall(float dt)
{
	const glm::vec3 gravityVec = { 0.f, this->gravity, 0.f };

	Transform* transform = this->host->getPausedTransform();
	transform->translate(this->pushDir*dt*this->pushSpeed + dt * gravityVec);

	transform->rotate(0.f, this->totalAngle*dt, 0.f);
	this->currentTime += dt;

	if (this->currentTime >= this->totalTime) {
		transform->setPosition({ 0.f, -100.f, 0.f });
		this->currentTime = this->totalTime;
	}
}
