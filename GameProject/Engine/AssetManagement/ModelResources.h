#pragma once

#include <Windows.h>
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

enum TextureType {
    TXTYPE_DIFFUSE = 0,
    TXTYPE_SPECULAR = 1,
};

struct Texture {
    GLuint id;
    TextureType type;
    std::string path;
};

struct Material {
    glm::vec3 Ka;
    glm::vec3 Ks;
    std::vector<Texture> Textures;
};
