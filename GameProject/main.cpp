#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Game/Game.h"
#include "Engine/AssetManagement/ModelLoader.h"

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H  

#define STB_IMAGE_IMPLEMENTATION
#include "../../Utils/stb_image.h" //Single library for img loader

#include "Engine/Sound/Sound.h"
#include "Engine/Sound/SoundContext.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();

	SoundContext::initSoundContext();
	Sound sound(1.0f, 1.0f, glm::vec3(0,0,0), glm::vec3(0,0,0), false);

	sound.loadSound("Assets/rock.wav");
	sound.playSound();



	Game game;
	game.start();
	
	Logger::destroy();

	ModelLoader::unloadModels();
	TextureManager::unloadTextures();

	return 0;
}
