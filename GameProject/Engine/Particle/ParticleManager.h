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
public:
	//Update all emitters
	void update(float dt);
	/*
		Add emitter
	*/
	void addEmitter(ParticleEmitter* emitter);
	int getMaxParticles() const;
	int getParticleCount() const;
	void updateBuffer();
};