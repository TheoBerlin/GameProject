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
	Copy the texture from 'other' to this.
	*/
	Texture(const Texture& other);

	/*
	Copy the texture from 'other' to this.
	*/
	Texture& operator=(const Texture& other);

	/*
	Create and load a texture from disc.
	Arguments:
		fileName: The path and name of the file.
		texType: The texture type. Eg. diffuse, specular etc.
		internalFormat: The format it will have when loaded by openGL.
	*/
	Texture(const std::string& fileName, unsigned internalFormat = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

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
	Texture(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
	virtual ~Texture();

	/*
	Delete the previous texture and loads a new from disc.
	Arguments:
		fileName: The path and name of the file.
		texType: The texture type. Eg. diffuse, specular etc.
		internalFormat: The format it will have when loaded by openGL.
	*/
	void reload(const std::string& fileName, unsigned internalFormat = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

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
	void recreate(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

	/*
	Update textre data with new information.
	Arguments:
		data: The data to be loaded by openGL.
		width: The width in pixels of the texture.
		height: The height in pixels of the texture.
		internalFormat: The format it will have when loaded by openGL.
		format: The format of the data which will be loaded.
		texType: The texture type. Eg. diffuse, specular etc.
	*/
	void update(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

	/*
		Resize existing texture
	*/
	void resize(unsigned int width, unsigned int height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);

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
	Get the path to the texture data.
	*/
	std::string getPath() const;

	/*
	Get the internal format of the texture.
	*/
	GLuint getInternalFormat() const;

	/*
	Get the format of the data which was passed to the texture.
	*/
	GLuint getFormat() const;
	
	/*
	Return true if the data was loaded correctly, otherwise false.
	*/
	bool hasLoadedData() const;

	void bind();
	void unbind();

private:
	void copyData(const Texture& other);
	void copyTextureData(const Texture& other);
	void loadImage(const std::string& fileName, unsigned internalFormat = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
	void init(unsigned char* data, unsigned int width, unsigned int height, unsigned internalFormat, unsigned format, GLenum type);
	void setParameters();

	GLuint id;
	GLuint internalFormat;
	GLuint format;
	GLenum type;
	std::string path;

	unsigned int width;
	unsigned int height;

	bool loaded;
};
