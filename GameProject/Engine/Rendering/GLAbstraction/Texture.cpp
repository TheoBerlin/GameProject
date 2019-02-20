#include "Texture.h"

#include <Utils/Logger.h>
#include <Utils/stb_image.h>

#include "GLFormats.h"

Texture::Texture() : id(0), loaded(false)
{
}

Texture::Texture(const Texture & other)
{
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

void Texture::update(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, TextureType texType)
{
	this->internalFormat = (GLuint)internalFormat;
	this->format = (GLuint)format;
	this->type = texType;
	this->width = width;
	this->height = height;
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void Texture::resize(unsigned int width, unsigned int height, unsigned internalFormat, unsigned format)
{
	this->internalFormat = (GLuint)internalFormat;
	this->format = (GLuint)format;
	this->width = width;
	this->height = height;
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
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
	this->type = other.getType();
	this->width = other.getWidth();
	this->height = other.getHeight();
	this->internalFormat = other.getInternalFormat();
	this->format = other.getFormat();
	this->loaded = other.loaded;
	
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

void Texture::loadImage(const std::string & fileName, TextureType texType, unsigned internalFormat)
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
	init(data, width, height, internalFormat, GL_RGBA, texType);

	// Free image data as OpenGL is storing a copy
	stbi_image_free(data);

	this->loaded = true;
}

void Texture::init(unsigned char * data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, TextureType texType)
{
	this->internalFormat = (GLuint)internalFormat;
	this->format = (GLuint)format;
	this->type = texType;
	this->width = width;
	this->height = height;
	// Use image data to create an OpenGL texture
	glGenTextures(1, &this->id);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
