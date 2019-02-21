#include "TextureManager.h"

#include <Utils/Logger.h>

std::map<std::string, Texture*> TextureManager::loadedTextures = std::map<std::string, Texture*>();

Texture* TextureManager::loadTexture(const std::string& fileName, TextureType type)
{
    std::map<std::string, Texture*>::iterator textureIterator;

    // See if the texture has already been loaded
    textureIterator = loadedTextures.find(fileName);

    if (textureIterator != loadedTextures.end()) {
        // The texture has already been loaded, return it
        LOG_INFO("The texture [%s] has already been loaded", fileName.c_str());
        return textureIterator->second;
    }

	// Load texture from disc and stor the data with openGL.
	Texture* newTexture = new Texture(fileName, type);

	// Check if texture was loaded.
	if (newTexture->hasLoadedData() == false)
	{
		delete newTexture;
		return nullptr;
	}

    // Store the new texture in the texture map
    loadedTextures[fileName] = newTexture;

    return newTexture;
}

Texture * TextureManager::getTexture(const std::string & fileName)
{
	std::map<std::string, Texture*>::iterator it = loadedTextures.find(fileName);
	if (it != loadedTextures.end())
		return it->second;
	LOG_WARNING("Texture not loaded: %s", fileName.c_str());
	return nullptr;
}

void TextureManager::unloadAllTextures()
{
    for (std::map<std::string, Texture*>::iterator itr = loadedTextures.begin(); itr != loadedTextures.end(); itr++) {
        delete itr->second;
    }

    loadedTextures.clear();
}

size_t TextureManager::textureCount()
{
    return loadedTextures.size();
}
