#include "VertexArray.h"

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

	bind();
	vbo->bind();
	for (size_t i = 0; i < attributes.attribs.size(); i++) {
		AttributeSettings attrib = attributes.attribs[i];
		glEnableVertexAttribArray(this->nextLocation);
		//vbo->setLocation(attrib.location, i);
		glVertexAttribPointer(this->nextLocation, attrib.size, GL_FLOAT, GL_FALSE, attributes.stride * sizeof(float), (void*)attrib.offset);
		this->nextLocation++;
	}
	vbo->unbind();

	vbo->setAttribCount(attributes.attribs.size());
}
/*
void VertexArray::setIndexBuffer(IndexBuffer* indexBuffer)
{
	this->indexBuffer = indexBuffer;
}*/

void VertexArray::bind()
{
	glBindVertexArray(this->id);
	/*
	for (auto vbo : this->vbos) {
		for (unsigned int i = 0; i < vbo->getAttribCount(); i++) {
			glEnableVertexAttribArray(vbo->getLocation(i));
		}
	}
	*/
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
	/*
	for (auto vbo : this->vbos) {
		for (unsigned int i = 0; i < vbo->getAttribCount(); i++) {
			glDisableVertexAttribArray(vbo->getLocation(i));
		}
	}*/
}
