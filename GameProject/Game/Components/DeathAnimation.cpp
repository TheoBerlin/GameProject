#include "DeathAnimation.h"

#include <Engine/Entity/Entity.h>

DeathAnimation::DeathAnimation(Entity * host) : Component(host, "DeathAnimation")
{
	this->totalTime = 0.f;
}

DeathAnimation::~DeathAnimation()
{
}

void DeathAnimation::update(const float & dt)
{
	const glm::vec3 gravity = { 0.f, -0.9f, 0.f };
	if (this->totalTime > 0.f) {
		Transform* transform = this->host->getPausedTransform();
		transform->translate(this->dir*dt*this->speed + dt*gravity);
		this->totalTime -= dt;

		if (this->totalTime <= 0.f) {
			transform->setPosition({ 0.f, -100.f, 0.f });
		}
	}
}

void DeathAnimation::play(const glm::vec3& dir, float speed)
{
	this->totalTime = 0.25f;
	this->host->pauseModelTransform();

	this->dir = dir;
	this->speed = speed;
}

void DeathAnimation::reset()
{
	this->host->unpauseModelTransform();
}
