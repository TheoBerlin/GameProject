#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Game/Game.h"
#include "Engine/AssetManagement/ModelLoader.h"

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <Utils/stb_image.h> //Single library for img loader

#include "Engine/Sound/SoundContext.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();
	SoundContext::initSoundContext();

	Game game;
	game.start();
	
	Logger::destroy();

	ModelLoader::unloadAllModels();
	TextureManager::unloadAllTextures();

	return 0;
}
