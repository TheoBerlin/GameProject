#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned short>* vertexIndices, unsigned short materialIndex)
    : vertices(vertices),
    vertexIndices(vertexIndices),
    materialIndex(materialIndex)
{
    createBuffers();
}

Mesh::~Mesh()
{
    // Delete vertices and indices
    delete vertices;
    delete vertexIndices;
}

void Mesh::createBuffers()
{
}
