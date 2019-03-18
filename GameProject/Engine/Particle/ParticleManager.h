#pragma once

#include "ParticleEmitter.h"
#include <vector>
#include <unordered_map>
#include "GL/glew.h"
#include "Engine/Rendering/GLAbstraction/VertexArray.h"
#include "Engine/Rendering/GLAbstraction/VertexBuffer.h"

#include <thread>

class ParticleManager {
private:
	std::vector<ParticleEmitter*> emitters;

	void updateEmitter(ParticleEmitter *emitter, float dt);

	ParticleManager() = default;
	ParticleManager(const ParticleManager& other) = delete;

	bool hasParticlesVisble{ false };

	static const GLfloat g_vertex_buffer_data[16];
	VertexArray va;
	VertexBuffer* vbBillboard = nullptr;
	VertexBuffer* vbParticle = nullptr;

public:
	static ParticleManager& get();
	void init();

	//Update all emitters
	void update(float dt);

	virtual ~ParticleManager();

	/*
		Add emitter, returns index of emitter vector
	*/
	void addEmitter(ParticleEmitter* emitter);

	/*
		Remove Emitter with index in emitter vector
	*/
	void removeEmitter(ParticleEmitter* emitter);

	unsigned getMaxParticles() const;
	unsigned getParticleCount() const;
	VertexBuffer* getVertexBuffer() const;
	VertexArray* getVertexArray();
	void updateBuffer();

	bool hasVisibleParticles() const;
};