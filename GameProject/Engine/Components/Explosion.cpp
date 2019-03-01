#include "Explosion.h"

#include "Engine/Entity/Entity.h"

Explosion::Explosion(Entity * host, unsigned explosionDebris)
	: Component(host, "Explosion")
{
	ParticleManager& pm = ParticleManager::get();

	glm::vec3 entityPos = this->host->getTransform()->getPosition();

	createDebri(entityPos, glm::vec3(1.0, 1.0, 0.0));
	//createDebri(entityPos + glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 1.0, 1.0));
	//createDebri(entityPos + glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 1.0, -1.0));
}

Explosion::~Explosion()
{
	for (auto* debri : this->debris) {
		//Remove emitter
		ParticleManager::get().removeEmitter(debri->emitter);

		delete debri;
	}
}

void Explosion::update(const float & dt)
{
	for (auto* debri : this->debris) {
		debri->position += debri->velocity * dt;
		debri->velocity += debri->acceleration * dt;
		debri->emitter->setPosition(debri->position);
		//debri->emitter->setVelocity(-debri->velocity);
	}
		
}

void Explosion::createDebri(const glm::vec3& pos, const glm::vec3& startVelocity, const glm::vec3& startAcceleration)
{
	ParticleEmitter* emitter = new ParticleEmitter();
	Debri* debri = new Debri();
	debri->position = pos;
	debri->velocity = startVelocity;
	debri->acceleration = startAcceleration;
	debri->emitter = emitter;
	

	emitter->setPosition(pos);
	emitter->setSpread(0.2f);
	//emitter->setVelocity(-startVelocity);
	emitter->setAcceleration(startAcceleration);
	emitter->setMaxParticle(30);
	emitter->setSpawnRate(10);
	emitter->setStartColour(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	emitter->setEndColour(glm::vec4(0.2f, 0.2f, 0.2f, 0.0f));
	emitter->setLifeTime(3);
	emitter->setScaleChange(1.0f);
	emitter->setScale(0.5f);

	emitter->playEmitter(2);

	ParticleManager::get().addEmitter(emitter);
	this->debris.push_back(debri);
}
