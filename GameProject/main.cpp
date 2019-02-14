#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Utils/Settings.h"
#include "Game/Game.h"
#include "Engine/AssetManagement/ModelLoader.h"

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <Utils/stb_image.h> //Single library for img loader

#include "Engine/Sound/SoundContext.h"
#include "Engine/Sound/Sound.h"
#include "Utils/Settings.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();
	SoundContext::initSoundContext();
	
	/*Sound sound;
	sound.loadSound("Game/assets/sound/dream_catcher.wav");
	sound.setLoopState(true);
	sound.playSound();*/

	Game game;
	game.start();
	
	Logger::destroy();

	ModelLoader::unloadAllModels();
	TextureManager::unloadAllTextures();

	return 0;
}
