#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
    IndexBuffer(const void* const data, const size_t& dataSize);
    ~IndexBuffer();

private:
    GLuint id;
    size_t dataSize;
};
