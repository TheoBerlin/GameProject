#include "Explosion.h"

#include "Engine/Entity/Entity.h"
#include <cstdlib>
#include <ctime>

Explosion::Explosion(Entity * host, unsigned explosionDebris)
	: Component(host, "Explosion")
{
	ParticleManager& pm = ParticleManager::get();

	glm::vec3 entityPos = this->host->getTransform()->getPosition();

	srand(static_cast <unsigned> (time(0)));
	float LO = -1.0;
	float HI = 1.0;

	const float speed = 1.5;
	const glm::vec3 gravity(0.0, -3.0, 0.0);

	const size_t debriAmount = 5;
	for (size_t i = 0; i < debriAmount; i++) {
		float rx = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		float rz = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		float rSpeed = 1.0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0 - 1.0)));
		createDebri(entityPos, glm::vec3(rx, 1.0, rz) * rSpeed, gravity, glm::vec3(0.8f), 0.15, 0.5);
	}

	createDebri(entityPos, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.1f), 0.35, 2.0, 0.25);
	
	this->timer = 0.0;
}

Explosion::~Explosion()
{
	this->removeDebris();
}

void Explosion::update(const float & dt)
{
	for (auto* debri : this->debris) {
		debri->position += debri->velocity * dt;
		debri->velocity += debri->acceleration * dt;
		debri->emitter->setPosition(debri->position);
		//debri->emitter->setVelocity(-debri->velocity);
	}

	this->timer += dt;
	if (this->timer > 2.0) {
		this->removeDebris();
		this->timer = 0.0;
	}
		
		
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
	//emitter->setVelocity(-startVelocity);
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
