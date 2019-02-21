#pragma once

#include "GL/glew.h"

#include <Utils/stb_image.h>
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include "../Rendering/GLAbstraction/Texture.h"
#include <map>

class TextureManager
{
public:

	/*
	Loads a texture from file or finds the already loaded texture.
	Arguments:
		fileName: The path to the file.
		type: The type of texture. E.g Diffuse, specular etc.
	Return:
		nullptr if not found else a pointer to the texture.
	*/
    static Texture* loadTexture(const std::string& fileName, TextureType type = TextureType::TXTYPE_DIFFUSE);

	/*
	Get a stored texture.
	Arguments:
		fileName: Path and name to file.
	Return:
		nullptr if not found else a pointer to the Texture.
	*/
	static Texture* getTexture(const std::string& fileName);

	/*
	Free all loaded textures from memory.
	*/
    static void unloadAllTextures();

	/*
	Get the number of textures loaded.
	*/
    static size_t textureCount();

private:
    static std::map<std::string, Texture*> loadedTextures;
};
