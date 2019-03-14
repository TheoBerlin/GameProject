#include "VertexArray.h"

#include "Utils/Logger.h"

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
	this->buffers.push_back(vbo);

	vbo->bind();
	bind();
	for (size_t i = 0; i < attributes.attribs.size(); i++) {
		AttributeSettings attrib = attributes.attribs[i];

		glEnableVertexAttribArray(this->nextLocation);
		glVertexAttribPointer(this->nextLocation, attrib.size, GL_FLOAT, GL_FALSE, attributes.stride * sizeof(float), (void*)attrib.offset);
		glVertexAttribDivisor(this->nextLocation, attrib.divisor);
		this->nextLocation++;
	}
	vbo->unbind();

	vbo->setAttribCount(attributes.attribs.size());

	
}

void VertexArray::updateBuffer(unsigned vboIndex, const void* data, const size_t dataSize, unsigned offset, const unsigned& usage)
{
	this->bind();
	if (vboIndex < this->buffers.size()) {

		VertexBuffer* vbo = this->buffers[vboIndex];
		if (dataSize + offset <= vbo->getDataSize()) {

			vbo->updateData(data, dataSize, offset);
		}
		else {
			vbo->make(data, dataSize + offset, usage);
		}

	}

}

void VertexArray::setBuffer(unsigned vboIndex, const void * data, const size_t dataSize, const unsigned& usage)
{
	this->bind();
	if (vboIndex < this->buffers.size()) {

		VertexBuffer* vbo = this->buffers[vboIndex];
		
		vbo->make(data, dataSize, usage);
	}
}


void VertexArray::bind()
{
	glBindVertexArray(this->id);
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
}
