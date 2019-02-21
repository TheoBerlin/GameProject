#pragma once

#include "ParticleEmitter.h"
#include <vector>
#include <unordered_map>
#include "GL/glew.h"

#include <thread>

class ParticleManager {
private:
	std::vector<ParticleEmitter*> emitters;

	void updateEmitter(ParticleEmitter *emitter, float dt);

	ParticleManager() = default;
	ParticleManager(const ParticleManager& other) = delete;
public:
	static ParticleManager& get();

	//Update all emitters
	void update(float dt);

	virtual ~ParticleManager() {}
	/*
		Add emitter
	*/
	void addEmitter(ParticleEmitter* emitter);
	int getMaxParticles() const;
	int getParticleCount() const;
	void updateBuffer();
};