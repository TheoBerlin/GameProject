#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Game/Game.h"
#include "Engine/AssetManagement/ModelLoader.h"

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();
	
	Game game;
	game.start();

	
	Logger::destroy();

	ModelLoader::unloadModels();
	TextureManager::unloadTextures();

	return 0;
}
