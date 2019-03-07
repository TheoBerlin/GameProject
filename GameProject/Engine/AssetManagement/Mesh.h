#pragma once

#include <vector>
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "../Rendering/GLAbstraction/IndexBuffer.h"
#include "../Rendering/GLAbstraction/UniformBuffer.h"

class Model;
class Mesh
{
public:
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex, Model* parent, GLenum usage = GL_STATIC_DRAW);
	Mesh(const void * data, size_t dataSize, const void * indices, size_t indicesSize, AttributeLayout layout, GLenum usage = GL_STATIC_DRAW);
    ~Mesh();

    void bindVertexArray();
	IndexBuffer& getIndexBuffer();

    // Inserts material data into a uniform buffer
    void bindMaterial(UniformBuffer* uniformBuffer);
    unsigned short getMaterialIndex();

	void addBuffer(const void * data, size_t dataSize, const AttributeLayout& layout);

	void initInstancing(const void * data, size_t dataSize);
	void initInstancing(const void * data, size_t dataSize, const AttributeLayout& attributeLayout);

	void updateInstancingData(const void* data, size_t dataSize, unsigned offset = 0, unsigned buffer = 1);

	// Get verticies
	std::vector<Vertex>& getVerticies() const;
	// Get indicies
	std::vector<unsigned>& getIndicies() const;

private:
    Model* parentModel;
    VertexArray* vao;
	IndexBuffer* ib;

    std::vector<Vertex>* vertices;
    std::vector<unsigned int>* vertexIndices;
    // The index refers to the material storage in the mesh's model
    unsigned short materialIndex;
};
