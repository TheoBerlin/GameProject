#pragma once

#include <vector>

#include "VertexBuffer.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	/*
		Adds an Vbo to be stored in the Vao, use layout to specify settings of vbo in shader
		Important to push attributes to attributeHandler in order of how they lay in the vertex buffer
	*/
void addBuffer(VertexBuffer* vbo, const AttributeLayout& layout);

	/*
		Update existing buffer, with new layout, if size don't use default parameter the whole buffer will be reallocated and new memory has to be sent in
	*/
	void updateBuffer(unsigned vboIndex, const void* data, const size_t dataSize, unsigned offset = 0, const unsigned& usage = GL_DYNAMIC_DRAW);

	/*
		Set buffer usage, data and data size
	*/
	void setBuffer(unsigned vboIndex, const void* data, const size_t dataSize, const unsigned& usage = GL_DYNAMIC_DRAW);

	/*
		used before drawing
	*/
	void bind();
	void unbind();

private:
	unsigned id;
	unsigned nextLocation;
	std::vector<VertexBuffer*> buffers;
};
