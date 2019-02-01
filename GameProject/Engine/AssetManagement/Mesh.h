#pragma once

#include <vector>
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include "../Rendering/GLAbstraction/VertexArray.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex);
    ~Mesh();

    void bindVertexBuffer();
    void bindTextures();
    void bindMaterial();

private:
    VertexArray* VAO;

    std::vector<Vertex>* vertices;
    std::vector<unsigned int>* vertexIndices;
    // The index refers to the material storage in the mesh's model
    unsigned short materialIndex;
};
