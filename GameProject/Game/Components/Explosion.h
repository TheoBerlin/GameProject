#pragma once

#include "Engine/Particle/ParticleManager.h"
#include "Engine/Components/Component.h"
#include "Engine/Sound/SoundManager.h"

struct Debri {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	ParticleEmitter* emitter;
};

class Explosion : public Component
{
public: 
	Explosion(Entity* host);
	~Explosion();

	void update(const float& dt);

	void explode(float lifeTime = 3.0, float timeElapsed = 0.0, bool eyeHit = false, unsigned explosionDebris = 3, float speed = 1.5, float gravity = -3.0);

	void reset();

private:
	void createDebri(const glm::vec3& pos, const glm::vec3& startVelocity, const glm::vec3& startAcceleration = glm::vec3(0.0f, -1.0f, 0.0f)
		, const glm::vec3& color = glm::vec3(1.0f), const float& scale = 0.15, const float& spread = 0, const float& life = 0.5);
	void createEyeDebri(const glm::vec3& pos, const glm::vec3& startVelocity, const glm::vec3& startAcceleration = glm::vec3(0.0f, -1.0f, 0.0f)
		, const glm::vec3& color = glm::vec3(1.0f), const float& scale = 0.15, const float& spread = 0, const float& life = 0.5);
	void removeDebris();

	float lifeTime;
	float timer;
	unsigned debriAmount;
	std::vector<Debri*> debris;
	Sound sound;
};

