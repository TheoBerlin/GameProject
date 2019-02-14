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
	VertexBuffer(const void* const data, const size_t& dataSize, unsigned usage = GL_STATIC_DRAW);
	~VertexBuffer();

	unsigned getID() { return this->id; };

	void bind();
	void unbind();
	void updateData(const void* const data, const size_t& dataSize, int offset);

	//void setLocation(unsigned location, unsigned attribNumber) { this->locations[attribNumber] = location; };
	//unsigned getLocation(unsigned attribNumber) { return locations[attribNumber]; };

	void setAttribCount(unsigned count) { this->attribCount = count; };
	unsigned getAttribCount() { return this->attribCount; };

	void setDataSize(size_t size);
	size_t getDataSize() const;

private:
	unsigned id, attribCount;
	size_t dataSize;
	/*
		Locations for all attributes included in the vbo
	*/
	//unsigned locations[5];
};
