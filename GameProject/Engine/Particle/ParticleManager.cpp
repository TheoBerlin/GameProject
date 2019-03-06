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

	this->hasParticlesVisble = false;
}

void ParticleManager::update(float dt)
{
	for (int i = 0; i < emitters.size(); i++) {
		emitters[i]->update(dt);
	}
}

ParticleManager::~ParticleManager()
{
	for (auto* emitter : this->emitters)
		delete emitter;
}

void ParticleManager::addEmitter(ParticleEmitter* emitter)
{
	emitter->setEmitterVectorIndex(this->emitters.size());
	this->emitters.push_back(emitter);

	// Update vbo to new size
	this->va.setBuffer(1, NULL, this->getMaxParticles() * sizeof(Particle), GL_STREAM_DRAW);
}

void ParticleManager::removeEmitter(ParticleEmitter* emitter)
{
	size_t lastEmitterIndex = this->emitters.size() - 1;
	size_t index = emitter->getEmitterVectorIndex();

	if (index >= 0 && index <= lastEmitterIndex) {
		ParticleEmitter* pe = this->emitters[index];
		pe->clearEmitter();
		delete pe;
		
		//If emitter is not in last place
		if (index < lastEmitterIndex) {
			//get last emitter
			pe = this->emitters[lastEmitterIndex];

			//Swap last emitter with the emitter which will be removed
			this->emitters.pop_back();
			this->emitters[index] = pe;

			//Loop through emitters from removed emitter and update vector index
			while (index < this->emitters.size()) {
				this->emitters[index]->setEmitterVectorIndex(index);
				index++;
			}
		}
		else 
			this->emitters.pop_back();
	}

	// Update vbo to new size
	this->va.setBuffer(1, NULL, this->getMaxParticles() * sizeof(Particle), GL_STREAM_DRAW);

}

unsigned ParticleManager::getMaxParticles() const
{
	int maxParticle = 0;
	for (size_t i = 0; i < emitters.size(); i++) {
		maxParticle += emitters[i]->getMaxParticle();
	}
	return maxParticle;
}

unsigned ParticleManager::getParticleCount() const
{
	int countParticle = 0;
	for (size_t i = 0; i < emitters.size(); i++) {
		if(!emitters[i]->isDead())
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

	this->hasParticlesVisble = false;
	for (int i = 0; i < emitters.size(); i++) {
		size_t size = emitters[i]->getParticleArray().size();
		if (size > 0 && !emitters[i]->isDead()) {
			this->va.updateBuffer(1, &emitters[i]->getParticleArray().front(), size * sizeof(Particle), offset);
			offset += size * sizeof(Particle);
			this->hasParticlesVisble = true;
		}
		
	}

}

bool ParticleManager::hasVisibleParticles() const
{
	return this->hasParticlesVisble;
}
