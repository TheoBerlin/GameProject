#pragma once

#include <string>
#include <vector>

class UniformBuffer
{
public:
	/*
		Maximum amount of binding points is 20
	*/
	UniformBuffer(unsigned shaderID, const std::string& blockName, unsigned bindingPoint);
	~UniformBuffer();

	void setData(const void* const data, size_t dataSize);

	/*
		Replace specific part of uniform data, if offset(in bytes) + data size is bigger than the size of the uniform buffer return false and dosen't change anything
	*/
	bool setSubData(const void* const data, size_t dataSize, unsigned int offset);

private:
	unsigned id, bindingPoint, currentSize;
};
