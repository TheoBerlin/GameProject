#pragma once

#include <string>
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <vector>

/*
	drawOption choose between GL_STATIC_DRAW, GL_DYNAMIC_DRAW and GL_STREAM_DRAW
*/
struct AttributeSettings {
	AttributeSettings(unsigned size, unsigned offset = 0, unsigned divisor = 0)
		: size(size), offset(offset), divisor(divisor) {}

	unsigned size;
	unsigned offset;
	unsigned divisor;
};

struct AttributeLayout {

	AttributeLayout() { this->stride = 0; }

	/*
		Specifies new attribute in the vbo, calculates stride, offset and location depending on the pushes
		Location in shader is based upon pushing order
	*/
	void push(const unsigned& size, const unsigned& divisor = 0) {

		AttributeSettings attribute(size, this->stride*sizeof(float), divisor);
		this->stride += size;

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
	TXTYPE_NORMAL = 2
};

class Texture;
struct Material {
	glm::vec4 Kd;
	glm::vec4 Ks_factor;
	std::vector<Texture*> textures;
};
