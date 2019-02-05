#pragma once

#include <vector>

#include "VertexBuffer.h"
#include "IndexBuffer.h"

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
	void setIndexBuffer(IndexBuffer* indexBuffer);

	/*
		used before drawing
	*/
	void bind();
	void unbind();

private:
	unsigned id;

	std::vector<VertexBuffer*> vbos;
	IndexBuffer* indexBuffer;
};
