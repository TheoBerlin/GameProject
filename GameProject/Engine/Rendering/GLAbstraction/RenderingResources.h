#pragma once

#include <Windows.h>
#include <string>
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <vector>

/*
	drawOption choose between GL_STATIC_DRAW, GL_DYNAMIC_DRAW and GL_STREAM_DRAW
*/
struct AttributeSettings {
	AttributeSettings(unsigned size, unsigned offset = 0, unsigned location = 0)
		: size(size), offset(offset), location(location) {}

	unsigned size;
	unsigned offset;
	unsigned location;
};

struct AttributeLayout {

	AttributeLayout() { this->stride = 0; }

	/*
		Specifies new attribute in the vbo, calculates stride, offset and location depending on the pushes
		Location in shader is based upon pushing order
	*/
	void push(const unsigned& size) {

		this->stride += size;
		AttributeSettings attribute(size, this->stride, this->attribs.size());

		this->attribs.push_back(attribute);

	};

	unsigned stride;

	std::vector<AttributeSettings> attribs;
};

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
