#pragma once

#include <Windows.h>
#include "GL/glew.h"

#include <Utils/Logger.h>
#include <Utils/stb_image.h>
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include <map>

class TextureManager
{
public:
    // Loads a texture from file or finds the already loaded texture
    static Texture* loadTexture(std::string fileName, TextureType type);

    static void unloadAllTextures();
    static size_t textureCount();

private:
    static std::map<std::string, Texture*> loadedTextures;
};
