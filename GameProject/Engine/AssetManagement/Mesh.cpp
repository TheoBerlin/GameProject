#include "Mesh.h"

#include "Model.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex, Model* parent)
    : vertices(vertices),
    vertexIndices(vertexIndices),
    materialIndex(materialIndex),
    parentModel(parent)
{
    VertexBuffer VBO((void*)&vertices[0], vertices->size() * sizeof(Vertex));
    IndexBuffer indexBuffer((void*)&vertexIndices[0], vertexIndices->size() * sizeof(unsigned short));

    VAO = new VertexArray();
}

void Mesh::bindVertexBuffer()
{
    VAO->bind();
}

void Mesh::bindMaterial(UniformBuffer* uniformBuffer)
{
    Material* material = &parentModel->getMaterial(this->materialIndex);
    uniformBuffer->setData((void*)material, sizeof(*material) - sizeof(material->Textures));
}

Mesh::~Mesh()
{
    // Delete vertices and indices
    delete vertices;
    delete vertexIndices;

    delete VAO;
}

unsigned short Mesh::getMaterialIndex()
{
    return this->materialIndex;
}

void Mesh::createBuffers()
{
}
