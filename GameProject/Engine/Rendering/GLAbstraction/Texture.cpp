#include "Texture.h"

#include <Utils/Logger.h>
#include <Utils/stb_image.h>

Texture::Texture() : id(0)
{
}

Texture::Texture(const std::string & fileName, TextureType texType, unsigned internalFormat)
{
	loadImage(fileName, texType, internalFormat);
}

Texture::Texture(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, TextureType texType) : id(0)
{
	init(data, width, height, internalFormat, format, texType);
}

Texture::~Texture()
{
	if(this->id != 0)
		glDeleteTextures(1, &this->id);
}

void Texture::reload(const std::string & fileName, TextureType texType, unsigned internalFormat)
{
	if (this->id != 0)
		glDeleteTextures(1, &this->id);
	loadImage(fileName, texType, internalFormat);
}

void Texture::recreate(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, TextureType texType)
{
	if (this->id != 0)
		glDeleteTextures(1, &this->id);
	init(data, width, height, internalFormat, format, texType);
}

unsigned int Texture::getWidth() const
{
	return this->width;
}

unsigned int Texture::getHeight() const
{
	return this->height;
}

GLuint Texture::getID() const
{
	return this->id;
}

TextureType Texture::getType() const
{
	return this->type;
}

void Texture::setType(TextureType type)
{
	this->type = type;
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadImage(const std::string & fileName, TextureType texType, unsigned internalFormat)
{
	this->path = fileName;
	int width, height, channelCount;
	// Load image data
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &channelCount, 4);

	if (!data)
	{
		LOG_WARNING("Unable to load texture [%s]", fileName.c_str());
		return;
	}

	this->width = width;
	this->height = height;
	init(data, width, height, internalFormat, GL_RGBA, texType);

	// Free image data as OpenGL is storing a copy
	stbi_image_free(data);
}

void Texture::init(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, TextureType texType)
{
	this->type = texType;
	// Use image data to create an OpenGL texture
	glGenTextures(1, &this->id);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	setParameters();
}

void Texture::setParameters()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
