#include "UniformBuffer.h"

#include "GL/glew.h"

#include "../../../Utils/Logger.h"

UniformBuffer::UniformBuffer(unsigned shaderID, const std::string & blockName, unsigned bindingPoint)
{
	glGenBuffers(1, &this->id);
	this->bindingPoint = bindingPoint;

}

UniformBuffer::~UniformBuffer()
{
}

void UniformBuffer::setData(const std::vector<float>& data)
{
	this->currentSize = data.size() * sizeof(float);

	glBindBuffer(GL_UNIFORM_BUFFER, this->id);

	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)* data.size(), &data[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, this->bindingPoint, this->id);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool UniformBuffer::setSubData(const std::vector<float>& data, unsigned offset)
{
	if (offset + data.size() * sizeof(float) < this->currentSize) {
		glBindBuffer(GL_UNIFORM_BUFFER, this->id);

		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float)* data.size(), &data[0]);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	LOG_WARNING("Data don't fit in uniform buffer! Data size: %d Free Data from offset: %d", data.size() * sizeof(float), this->currentSize - offset);
	return false;
}

