#include "ParticleManager.h"

const GLfloat ParticleManager::g_vertex_buffer_data[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};


void ParticleManager::updateEmitter(ParticleEmitter* emitter, float dt)
{
	emitter->update(dt);
}

ParticleManager & ParticleManager::get()
{
	static ParticleManager particleManager;
	return particleManager;
}

void ParticleManager::init()
{
	this->va.bind();

	this->vbBillboard = new VertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data), GL_STATIC_DRAW);
	AttributeLayout layout;
	layout.push(3); // Vertex
	this->va.addBuffer(this->vbBillboard, layout);

	this->vbParticle = new VertexBuffer(NULL, ParticleManager::get().getMaxParticles() * sizeof(Particle), GL_STREAM_DRAW);
	AttributeLayout layout2;
	layout2.push(4, 1); // Pos / scale
	layout2.push(4, 1); // Colour
	this->va.addBuffer(this->vbParticle, layout2);
}

void ParticleManager::update(float dt)
{
	for (int i = 0; i < emitters.size(); i++) {
		emitters[i]->update(dt);
	}
}

void ParticleManager::addEmitter(ParticleEmitter* emitter)
{
	emitters.push_back(emitter);
}

int ParticleManager::getMaxParticles() const
{
	int maxParticle = 0;
	for (int i = 0; i < emitters.size(); i++) {
		maxParticle += emitters[i]->getMaxParticle();
	}
	return maxParticle;
}

int ParticleManager::getParticleCount() const
{
	int countParticle = 0;
	for (int i = 0; i < emitters.size(); i++) {
		countParticle += emitters[i]->getParticleArray().size();
	}
	return countParticle;
}

VertexBuffer * ParticleManager::getVertexBuffer() const
{
	return vbParticle;
}

VertexArray * ParticleManager::getVertexArray()
{
	return &va;
}

void ParticleManager::updateBuffer()
{
	int offset = 0;
	for (int i = 0; i < emitters.size(); i++) {
		if (emitters[i]->getParticleArray().size() > 0) {
			glBufferSubData(GL_ARRAY_BUFFER, offset, emitters[i]->getParticleArray().size() * sizeof(Particle), &emitters[i]->getParticleArray().front());
			offset += emitters[i]->getParticleArray().size() * sizeof(Particle);
		}
	}
}