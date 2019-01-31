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

	void setData(const std::vector<float>& data);

	/*
		Replace specific part of uniform data, if offset(in bytes) + data size is bigger than the size of the uniform buffer return false and dosen't change anything
	*/
	bool setSubData(const std::vector<float>& data, unsigned offset);

private:
	unsigned id, bindingPoint, currentSize;
};

