#include "DeathAnimation.h"

#include <Engine/Entity/Entity.h>
#include <Utils/Utils.h>

DeathAnimation::DeathAnimation(Entity * host) : Component(host, "DeathAnimation")
{
	this->totalTime = 0.f;
	this->totalRotation = {0.f, 0.f, 0.f};
	this->totalTranslation = {0.f, 0.f, 0.f};
}

DeathAnimation::~DeathAnimation()
{
}

void DeathAnimation::update(const float & dt)
{
	if (this->currentTime < this->totalTime) {
		spasm(dt);
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
}

void DeathAnimation::spasm(float dt)
{
	spasmTimer += dt;

	spasmTimer = std::fmod(spasmTimer, glm::two_pi<float>());

	// Translate
	float translationFactor = 0.5f * sinf((spasmTimer - glm::half_pi<float>())) + 0.5f;

	glm::vec3 translation = translationFactor * maxTranslation;

	// Rotate
	float frequency = 30.f;
	float rotationFactor = 0.5f * sinf(spasmTimer * frequency) + 0.5f;

	glm::vec3 rotation = glm::mix(maxRotation, -maxRotation, rotationFactor);

	// Apply transformations
	Transform* transform = this->host->getPausedTransform();

	transform->translate(translation - totalTranslation);

	transform->rotate(rotation.x - totalRotation.x, rotation.y - totalRotation.y, rotation.z - totalRotation.z);

	// Store total amount of relative transformations applied by Hover
	totalTranslation = translation;
	totalRotation = rotation;
	/*
	glm::quat rot = transform->getRotationQuat();
	rot = Utils::rotate(rot, rotation.x, rotation.y, rotation.z);
	transform->setRotationQuat(rot);
	*/
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
