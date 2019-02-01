#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex)
    : vertices(vertices),
    vertexIndices(vertexIndices),
    materialIndex(materialIndex)
{
    VertexBuffer VBO((void*)&vertices[0], vertices->size() * sizeof(Vertex));
    IndexBuffer indexBuffer((void*)&vertexIndices[0], vertexIndices->size() * sizeof(unsigned short));

    VAO = new VertexArray();
}

Mesh::~Mesh()
{
    // Delete vertices and indices
    delete vertices;
    delete vertexIndices;

    delete VAO;
}
