#pragma once

#include "GL/glew.h"
#include "RenderingResources.h"
#include <string>

class Texture
{
public:
	/*
	Does nothing.
	*/
	Texture();

	/*
	Create and load a texture from disc.
	Arguments:
		fileName: The path and name of the file.
		texType: The texture type. Eg. diffuse, specular etc.
		internalFormat: The format it will have when loaded by openGL.
	*/
	Texture(const std::string& fileName, TextureType texType = TextureType::TXTYPE_DIFFUSE, unsigned internalFormat = GL_RGBA);

	/*
	Creates a texture from memory.
	Arguments:
		data: The data to be loaded by openGL.
		width: The width in pixels of the texture.
		height: The height in pixels of the texture.
		internalFormat: The format it will have when loaded by openGL.
		format: The format of the data which will be loaded.
		texType: The texture type. Eg. diffuse, specular etc.
	*/
	Texture(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA, TextureType texType = TextureType::TXTYPE_DIFFUSE);
	virtual ~Texture();

	/*
	Delete the previous texture and loads a new from disc.
	Arguments:
		fileName: The path and name of the file.
		texType: The texture type. Eg. diffuse, specular etc.
		internalFormat: The format it will have when loaded by openGL.
	*/
	void reload(const std::string& fileName, TextureType texType = TextureType::TXTYPE_DIFFUSE, unsigned internalFormat = GL_RGBA);

	/*
	Delete the previous texture and creates a new from memory.
	Arguments:
		data: The data to be loaded by openGL.
		width: The width in pixels of the texture.
		height: The height in pixels of the texture.
		internalFormat: The format it will have when loaded by openGL.
		format: The format of the data which will be loaded.
		texType: The texture type. Eg. diffuse, specular etc.
	*/
	void recreate(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA, TextureType texType = TextureType::TXTYPE_DIFFUSE);

	/*
		Resize existing texture
	*/
	void resize(unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA);

	/*
	Returns the width in pixels.
	*/
	unsigned int getWidth() const;
	
	/*
	Returns the height in pixels.
	*/
	unsigned int getHeight() const;

	/*
	Get the texture id.
	*/
	GLuint getID() const;

	/*
	Get the texture type. Eg. diffuse, specular, etc.
	*/
	TextureType getType() const;

	/*
	Set the texture type. Eg. diffuse, specular, etc.
	Arguments:
		type: The new type.
	*/
	void setType(TextureType type);

	void bind();
	void unbind();

private:
	void loadImage(const std::string& fileName, TextureType texType, unsigned internalFormat = GL_RGBA);
	void init(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, TextureType texType);
	void setParameters();

	GLuint id;
	TextureType type;
	std::string path;

	unsigned int width;
	unsigned int height;
};
