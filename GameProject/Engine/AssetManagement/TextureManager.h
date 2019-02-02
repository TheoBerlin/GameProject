#pragma once

#include <Windows.h>
#include "GL/glew.h"

#include "Utils/stb_image.h"
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include <map>

class TextureManager
{
public:
    // Loads a texture from file or finds the already loaded texture
    static Texture* loadTexture(std::string fileName, TextureType type);

    static void unloadTextures();

private:
    static std::map<std::string, Texture*> loadedTextures;
};
