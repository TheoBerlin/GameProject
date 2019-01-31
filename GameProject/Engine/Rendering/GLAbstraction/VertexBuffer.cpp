#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const std::vector<float>& data)
{
	this->data = data;
	glGenBuffers(1, &this->id);

	glBindBuffer(GL_ARRAY_BUFFER, this->id);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->data.size(), &this->data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &this->id);
}
