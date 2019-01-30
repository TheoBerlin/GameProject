#pragma once

#include <Windows.h>
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <string>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum TextureType {
    DIFFUSE = 0,
    SPECULAR = 1,
};

struct Texture {
    GLuint id;
    TextureType type;
    std::string path;
};
