#pragma once

#include <vector>
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "../Rendering/GLAbstraction/UniformBuffer.h"

class Mesh
{
public:
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex, Model* parent);
    ~Mesh();

    unsigned short getMaterialIndex();

private:
    void createBuffers();
    void bindVertexBuffer();
    // Inserts material data into a uniform buffer
    void bindMaterial(UniformBuffer* uniformBuffer);

private:
    Model* parentModel;
    VertexArray* VAO;

    std::vector<Vertex>* vertices;
    std::vector<unsigned int>* vertexIndices;
    // The index refers to the material storage in the mesh's model
    unsigned short materialIndex;
};
