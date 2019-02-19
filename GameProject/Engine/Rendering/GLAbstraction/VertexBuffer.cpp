#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() : dataSize(0)
{
	glGenBuffers(1, &this->id);
}

VertexBuffer::VertexBuffer(const void* const data, const size_t& dataSize, GLenum usage)
	:dataSize(dataSize)
{
	glGenBuffers(1, &this->id);
	make(data, dataSize, usage);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &this->id);
}

void VertexBuffer::make(const void * const data, const size_t & dataSize, GLenum usage)
{
	bind();
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, usage);
	unbind();
}

void VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->id);
}

void VertexBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::updateData(const void * const data, const size_t & dataSize, int offset)
{
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, dataSize, data);
}

void VertexBuffer::setDataSize(size_t size)
{
	this->dataSize = size;
}

size_t VertexBuffer::getDataSize() const
{
	return this->dataSize;
}
