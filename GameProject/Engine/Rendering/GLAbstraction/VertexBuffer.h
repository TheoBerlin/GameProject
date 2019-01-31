#pragma once

#include <vector>

#include "GL/glew.h"

#include "RenderingResources.h"

class VertexBuffer
{
public:
	/*
		Set up for vbo object data is templated
	*/
	VertexBuffer(const std::vector<float>& data);
	~VertexBuffer();

	unsigned getID() { return this->id; };

	void setLocation(unsigned location, unsigned attribNumber) { this->locations[attribNumber] = location; };
	unsigned getLocation(unsigned attribNumber) { return locations[attribNumber]; };

	void setAttribCount(unsigned count) { this->attribCount = count; };
	unsigned getAttribCount() { return this->attribCount; };

private:
	unsigned id, attribCount;
	/*
		Locations for all attributes included in the vbo
	*/
	unsigned locations[5];

	std::vector<float> data;
};

