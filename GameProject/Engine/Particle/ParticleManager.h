#pragma once

#include "ParticleEmitter.h"
#include <vector>
#include <unordered_map>
#include "GL/glew.h"

#include <thread>

static class ParticleManager {
private:
	static std::vector<ParticleEmitter*> emitters;

	void updateEmitter(ParticleEmitter *emitter, float dt);
public:
	//Update all emitters
	static void update(float dt);
	/*
		Add emitter
	*/
	static void addEmitter(ParticleEmitter* emitter);
	int getMaxParticles() const;
	int getParticleCount() const;
	void updateBuffer();
};