#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* const data, const size_t& dataSize)
	:dataSize(dataSize)
{
	glGenBuffers(1, &this->id);

	bind();
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	unbind();
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &this->id);
}

void VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VertexBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::updateDate(const void * const data, const size_t & dataSize, int offset)
{
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, dataSize, data);
}
