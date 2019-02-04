#include "Mesh.h"

#include "Model.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex, Model* parent)
    : vertices(vertices),
    vertexIndices(vertexIndices),
    materialIndex(materialIndex),
    parentModel(parent)
{

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
