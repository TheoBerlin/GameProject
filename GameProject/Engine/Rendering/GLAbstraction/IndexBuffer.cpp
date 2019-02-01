#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const void* const data, const size_t& dataSize)
    :dataSize(dataSize)
{
    glGenBuffers(1, &this->id);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataSize, data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &this->id);
}
