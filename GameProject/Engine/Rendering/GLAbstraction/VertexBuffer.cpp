#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* const data, const size_t& dataSize)
	:dataSize(dataSize)
{
	glGenBuffers(1, &this->id);

	glBindBuffer(GL_ARRAY_BUFFER, this->id);

	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &this->id);
}
