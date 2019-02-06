#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
    IndexBuffer(const void* const data, const size_t& count);
    ~IndexBuffer();

	void bind();
	void unbind();

	size_t getCount() const;

private:
    GLuint id;
	size_t count;
};
