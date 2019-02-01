#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &this->id);

}

VertexArray::~VertexArray()
{
	// Delete VBOs
	for (unsigned int i = 0; i < vbos.size(); i += 1) {
		delete vbos.at(i);
	}

	delete indexBuffer;

	glDeleteVertexArrays(1, &this->id);
}

void VertexArray::addBuffer(VertexBuffer * vbo, const AttributeLayout& attributes)
{
	this->vbos.push_back(vbo);

	glBindVertexArray(this->id);

	for (int i = 0; i < attributes.attribs.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo->getID());

		AttributeSettings attrib = attributes.attribs[i];

		vbo->setLocation(attrib.location, i);
		glVertexAttribPointer(attrib.location, attrib.size, GL_FLOAT, GL_FALSE, attributes.stride * sizeof(float), (void*)attrib.offset);

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vbo->setAttribCount(attributes.attribs.size());
}

void VertexArray::setIndexBuffer(IndexBuffer* indexBuffer)
{
	this->indexBuffer = indexBuffer;
}

void VertexArray::bind()
{
	glBindVertexArray(this->id);

	for (auto vbo : this->vbos) {
		for (int i = 0; i < vbo->getAttribCount(); i++) {
			glEnableVertexAttribArray(vbo->getLocation(i));
		}
	}

}

void VertexArray::unbind()
{
	glBindVertexArray(0);

	for (auto vbo : this->vbos) {
		for (int i = 0; i < vbo->getAttribCount(); i++) {
			glDisableVertexAttribArray(vbo->getLocation(i));
		}
	}
}


