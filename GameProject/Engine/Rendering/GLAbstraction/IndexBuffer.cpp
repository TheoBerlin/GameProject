#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const void* const data, const size_t& count)
    :count(count)
{
    glGenBuffers(1, &this->id);

	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	unbind();
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &this->id);
}

void IndexBuffer::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
}

void IndexBuffer::unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

size_t IndexBuffer::getCount() const
{
	return this->count;
}
