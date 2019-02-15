#include "Mesh.h"

#include "Model.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex, Model* parent)
    : vertices(vertices),
    vertexIndices(vertexIndices),
    materialIndex(materialIndex),
    parentModel(parent)
{
	// Load mesh to GPU.
    this->vao = new VertexArray();
	this->vao->bind();
	VertexBuffer* vbo = new VertexBuffer((void*)&((*vertices)[0].Position.x), vertices->size() * sizeof(Vertex));
	AttributeLayout layout;
	layout.push(3); // vec3 Position
	layout.push(3); // vec3 Normal
	layout.push(2); // vec2 Texture coords
	this->vao->addBuffer(vbo, layout);

	this->vao->bind();
	this->ib = new IndexBuffer((void*)&((*vertexIndices)[0]), vertexIndices->size());
}

void Mesh::bindVertexArray()
{
    this->vao->bind();
}

IndexBuffer & Mesh::getIndexBuffer()
{
	return *this->ib;
}

void Mesh::bindMaterial(UniformBuffer* uniformBuffer)
{
    Material* material = &parentModel->getMaterial(this->materialIndex);
    uniformBuffer->setSubData((void*)material, sizeof(*material) - sizeof(material->textures), 0);
}

Mesh::~Mesh()
{
    // Delete vertices and indices
    delete this->vertices;
    delete this->vertexIndices;

    delete this->vao;
	delete this->ib;
}

unsigned short Mesh::getMaterialIndex()
{
    return this->materialIndex;
}

void Mesh::initInstancing(const void * data, size_t dataSize)
{
	/*
		Creating vbo for model/transform matrices 
	*/
	this->vao->bind();
	VertexBuffer* vbo = new VertexBuffer(data, dataSize, GL_DYNAMIC_DRAW);
	AttributeLayout layout;
	layout.push(4, 1); // vec4 row one in matrix
	layout.push(4, 1); // vec4 row two in matrix
	layout.push(4, 1); // vec4 row three in matrix
	layout.push(4, 1); // vec4 row four in matrix
	this->vao->addBuffer(vbo, layout);

}

void Mesh::updateInstancingData(const void * data, size_t dataSize, unsigned offset)
{
	/*
		Update second vbo contaning the matrices
	*/
	this->vao->updateBuffer(1, data, dataSize, offset);
}

void Mesh::createBuffers()
{
}
