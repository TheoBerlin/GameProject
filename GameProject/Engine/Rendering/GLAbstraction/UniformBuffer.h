#pragma once

#include <string>
#include <vector>

#include "Shader.h"

class UniformBuffer
{
public:
	UniformBuffer();
	~UniformBuffer();
	/*
		Binds uniform buffer to binding point
	*/
	void bind(unsigned bindingPoint);
	/*
		Bind uniform buffer to bindingpoint and allocate space for the block buffer in shader given as input.
	*/
	void bindShader(unsigned shaderID, const char* blockName, unsigned bindingPoint);
	void setData(const void* const data, size_t dataSize);

	/*
		Replace specific part of uniform data, if offset(in bytes) + data size is bigger than the size of the uniform buffer return false and dosen't change anything
	*/
	bool setSubData(const void* const data, size_t dataSize, unsigned offset);

private:
	unsigned id, bindingPoint, currentSize;

};
