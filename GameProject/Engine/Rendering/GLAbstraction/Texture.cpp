#include "Texture.h"

#include <Utils/Logger.h>
#include <Utils/stb_image.h>

#include "GLFormats.h"

Texture::Texture() : id(0), loaded(false)
{
	this->type = GL_UNSIGNED_BYTE;
	this->width = 0;
	this->height = 0;
	this->format = GL_RGBA;
	this->internalFormat = GL_RGBA;
	init(NULL, 2, 2, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
}

Texture::Texture(const Texture & other) : id(0), loaded(false)
{
	this->type = GL_UNSIGNED_BYTE;
	this->width = 0;
	this->height = 0;
	this->format = GL_RGBA;
	this->internalFormat = GL_RGBA;
	copyData(other);
}

Texture & Texture::operator=(const Texture & other)
{
	if (this != &other)
	{
		copyData(other);
	}
	return *this;
}

Texture::Texture(const std::string & fileName, unsigned internalFormat, GLenum type)
{
	this->id = 0;
	this->type = type;
	this->width = 0;
	this->height = 0;
	this->format = GL_RGBA;
	this->internalFormat = internalFormat;
	this->loaded = false;
	loadImage(fileName, internalFormat, type);
}

Texture::Texture(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, GLenum type) : id(0)
{
	this->loaded = false;
	this->type = type;
	this->width = width;
	this->height = height;
	this->format = format;
	this->internalFormat = internalFormat;
	init(data, width, height, internalFormat, format, type);
}

Texture::~Texture()
{
	if(this->id != 0)
		glDeleteTextures(1, &this->id);
}

void Texture::reload(const std::string & fileName, unsigned internalFormat, GLenum type)
{
	if (this->id != 0)
		glDeleteTextures(1, &this->id);
	loadImage(fileName, internalFormat, type);
}

void Texture::recreate(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, GLenum type)
{
	if (this->id != 0)
		glDeleteTextures(1, &this->id);
	init(data, width, height, internalFormat, format, type);
}

void Texture::update(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, GLenum type)
{
	this->internalFormat = (GLuint)internalFormat;
	this->format = (GLuint)format;
	this->width = width;
	this->height = height;
	this->type = type;
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
}

void Texture::resize(unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, GLenum type)
{
	this->internalFormat = (GLuint)internalFormat;
	this->format = (GLuint)format;
	this->width = width;
	this->height = height;
	this->type = type;
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
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

std::string Texture::getPath() const
{
	return this->path;
}

GLuint Texture::getInternalFormat() const
{
	return this->internalFormat;
}

GLuint Texture::getFormat() const
{
	return this->format;
}

bool Texture::hasLoadedData() const
{
	return this->loaded;
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::copyData(const Texture & other)
{
	this->path = other.getPath();
	this->width = other.getWidth();
	this->height = other.getHeight();
	this->internalFormat = other.getInternalFormat();
	this->format = other.getFormat();
	this->loaded = other.loaded;
	this->type = other.type;
	
	// Copy texture data
	size_t size = this->width*this->height * Formats::getTextureFormatChannels(this->format);
	std::vector<unsigned char> data(size);
	memset(data.data(), 0, size * sizeof(unsigned char));

	if (this->id == 0)
		init(data.data(), this->width, this->height, this->internalFormat, this->format, this->type);
	else
		update(data.data(), this->width, this->height, this->internalFormat, this->format, this->type);

	copyTextureData(other);
}

void Texture::copyTextureData(const Texture & other)
{
	this->bind();
	glCopyImageSubData(other.getID(), GL_TEXTURE_2D, 0, 0, 0, 0, this->id, GL_TEXTURE_2D, 0, 0, 0, 0, other.getWidth(), other.getHeight(), 1);
}

void Texture::loadImage(const std::string & fileName, unsigned internalFormat, GLenum type)
{
	this->path = fileName;
	int width, height, channelCount;
	// Load image data
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &channelCount, 4);

	if (!data)
	{
		LOG_WARNING("Unable to load texture [%s]", fileName.c_str());
		this->loaded = false;
		return;
	}

	this->width = width;
	this->height = height;
	init(data, width, height, internalFormat, GL_RGBA, type);

	// Free image data as OpenGL is storing a copy
	stbi_image_free(data);

	this->loaded = true;
}

void Texture::init(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, GLenum type)
{
	this->internalFormat = (GLuint)internalFormat;
	this->format = (GLuint)format;
	this->width = width;
	this->height = height;
	this->type = type;
	// Use image data to create an OpenGL texture
	glGenTextures(1, &this->id);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
	setParameters();

	this->loaded = true;
}

void Texture::setParameters()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
