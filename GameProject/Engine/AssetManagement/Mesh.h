#pragma once

#include <vector>
#include "../Rendering/GLAbstraction/RenderingResources.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned short>* vertexIndices, unsigned short materialIndex);
    ~Mesh();

private:
    void createBuffers();

    GLuint vertexVBO, indexVBO;

    std::vector<Vertex>* vertices;
    std::vector<unsigned short>* vertexIndices;
    // The index refers to the material storage in the meshe's model
    unsigned short materialIndex;
};
