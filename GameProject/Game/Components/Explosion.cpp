#include "Explosion.h"

#include "Engine/Entity/Entity.h"
#include <cstdlib>
#include <ctime>

Explosion::Explosion(Entity * host)
	: Component(host, "Explosion")
{

}

Explosion::~Explosion()
{
	this->removeDebris();
}

void Explosion::update(const float & dt)
{
	for (auto* debri : this->debris) {
		debri->position += debri->velocity * dt + 0.5f * debri->acceleration * dt * dt;
		debri->velocity += debri->acceleration * dt;
		debri->emitter->setPosition(debri->position);
	}

	this->timer += dt;
	if (this->timer > lifeTime) {
		this->removeDebris();
		this->timer = 0.0f;
	}
		
		
}

void Explosion::explode(float lifeTime, float timeElapsed, unsigned explosionDebris, float speed, float grav)
{
	this->lifeTime = lifeTime;
	if (timeElapsed < this->lifeTime) {
		glm::vec3 entityPos = this->host->getTransform()->getPosition();

		srand(static_cast <unsigned> (time(0)));
		float LO = -1.0f;
		float HI = 1.0f;

		glm::vec3 gravity(0.0f, grav, 0.0f);

		for (size_t i = 0; i < explosionDebris; i++) {
			float rx = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			float rz = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
			float rSpeed = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - 1.0f)));
			createDebri(entityPos, glm::vec3(rx, 1.0f, rz) * rSpeed, gravity, glm::vec3(0.8f), 0.15f, 0.5f);
		}

		createDebri(entityPos, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.1f), 0.35f, 2.0f, 0.25f);

		//If explosion is triggered with rewind the time elapsed has to be accounted for
		this->update(timeElapsed);

		this->timer = timeElapsed;
	}
}

void Explosion::reset()
{
	this->removeDebris();
}

void Explosion::createDebri(const glm::vec3& pos, const glm::vec3& startVelocity, const glm::vec3& startAcceleration, const glm::vec3& color,
	const float& scale, const float& spread, const float& life)
{
	ParticleEmitter* emitter = new ParticleEmitter();
	Debri* debri = new Debri();
	debri->position = pos;
	debri->velocity = startVelocity;
	debri->acceleration = startAcceleration;
	debri->emitter = emitter;
	

	emitter->setPosition(pos);
	emitter->setSpread(spread);
	emitter->setAcceleration(startAcceleration);
	emitter->setMaxParticle(400);
	emitter->setSpawnRate(100);
	emitter->setStartColour(glm::vec4(color * 0.8f, 1.0f));
	emitter->setEndColour(glm::vec4(color * 0.2f, 0.5f));
	emitter->setLifeTime(life);
	emitter->setScale(scale);

	emitter->playEmitter(2);

	ParticleManager::get().addEmitter(emitter);
	this->debris.push_back(debri);
}

void Explosion::removeDebris()
{
	for (auto* debri : this->debris) {
		//Remove emitter
		ParticleManager::get().removeEmitter(debri->emitter);

		delete debri;
	}
	this->debris.clear();
}
