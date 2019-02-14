#include "VertexArray.h"

#include "Utils/Logger.h"

VertexArray::VertexArray() : nextLocation(0)
{
	glGenVertexArrays(1, &this->id);
}

VertexArray::~VertexArray()
{
	// Delete VBOs
	for (unsigned int i = 0; i < vbos.size(); i += 1) {
		delete vbos.at(i);
	}

	//delete indexBuffer;

	glDeleteVertexArrays(1, &this->id);
}

void VertexArray::addBuffer(VertexBuffer * vbo, const AttributeLayout& attributes)
{
	this->vbos.push_back(vbo);

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

void VertexArray::updateBuffer(unsigned vboIndex, const void* data, const size_t dataSize, unsigned offset)
{
	this->bind();
	if (vboIndex < this->vbos.size()) {

		VertexBuffer* vbo = this->vbos[vboIndex];
		if (dataSize + offset <= vbo->getDataSize()) {

			vbo->updateData(data, dataSize, offset);
		}
		else {
			vbo->bind();

			glBufferData(GL_ARRAY_BUFFER, dataSize + offset, data, GL_DYNAMIC_DRAW);	
			vbo->setDataSize(dataSize + offset);
		}

	}
	else {
		LOG_ERROR("Index out of range, no vbo is attached to that index!");

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
