#include "TextureManager.h"

std::map<std::string, Texture*> TextureManager::loadedTextures = std::map<std::string, Texture*>();

Texture* TextureManager::loadTexture(std::string fileName, TextureType type)
{
    std::map<std::string, Texture*>::iterator textureIterator;

    // See if the texture has already been loaded
    textureIterator = loadedTextures.find(fileName);

    if (textureIterator != loadedTextures.end()) {
        // The texture has already been loaded, return it
        return textureIterator->second;
    }

    // Load the texture
    int width, height, channelCount;

    // Load image data
    unsigned char* textureData = stbi_load(fileName.c_str(), &width, &height, &channelCount, 3);

    Texture* newTexture = new Texture();
    newTexture->path = fileName;
    newTexture->type = type;

    // Use image data to create an OpenGL texture
    glGenTextures(1, &newTexture->id);

    glBindTexture(GL_TEXTURE_2D, newTexture->id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    // Free image data as OpenGL is storing a copy
    stbi_image_free(textureData);

    // Store the new texture in the texture map
    loadedTextures[fileName] = newTexture;

    return newTexture;
}

void TextureManager::unloadTextures()
{
    for (std::map<std::string, Texture*>::iterator itr = loadedTextures.begin(); itr != loadedTextures.end(); itr++) {
        delete itr->second;
    }

    loadedTextures.clear();
}
