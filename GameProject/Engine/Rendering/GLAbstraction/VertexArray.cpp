#include "VertexArray.h"

VertexArray::VertexArray() : nextLocation(0)
{
	glGenVertexArrays(1, &this->id);
}

VertexArray::~VertexArray()
{
	for (VertexBuffer* vb : this->buffers)
		delete vb;
	this->buffers.clear();
	glDeleteVertexArrays(1, &this->id);
}

void VertexArray::addBuffer(VertexBuffer* vbo, const AttributeLayout& attributes)
{
	buffers.push_back(vbo);
	bind();
	vbo->bind();
	for (size_t i = 0; i < attributes.attribs.size(); i++) {
		AttributeSettings attrib = attributes.attribs[i];
		glEnableVertexAttribArray(this->nextLocation);

		glVertexAttribPointer(this->nextLocation, attrib.size, GL_FLOAT, GL_FALSE, attributes.stride * sizeof(float), (void*)attrib.offset);
		this->nextLocation++;
	}
	vbo->unbind();

	vbo->setAttribCount(attributes.attribs.size());
}

void VertexArray::bind()
{
	glBindVertexArray(this->id);
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
}
