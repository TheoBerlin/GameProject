
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

void Mesh::bindVertexBuffer()
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
	uniformBuffer->setData((void*)material, sizeof(*material) - sizeof(material->Textures));
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

void Mesh::createBuffers()
{
}