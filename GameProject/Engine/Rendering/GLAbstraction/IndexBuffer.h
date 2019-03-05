#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
    IndexBuffer(const void* const data, const size_t& count, GLenum usage = GL_STATIC_DRAW);
    ~IndexBuffer();

	void updateData(const void* const data, const size_t& count);

	void bind();
	void unbind();

	size_t getCount() const;

private:
    GLuint id;
	size_t count;
};
