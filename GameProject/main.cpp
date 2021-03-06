#include <crtdbg.h>

#include "Utils/Logger.h"

#include "Utils/Settings.h"
#include "Game/Game.h"
#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Config.h"

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <Utils/stb_image.h> //Single library for img loader

#include "Engine/Sound/SoundContext.h"
#include "Engine/Sound/Sound.h"
#include "Engine/Sound/SoundManager.h"
#include "Utils/Settings.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Logger::init();
	//LOG_FILTER_WARNING_ERROR;

	SoundContext::initSoundContext();

	ran.seed(std::random_device()());

	Sound sound;
	sound.loadSound("Game/assets/sound/dream_catcherMono.wav");
	sound.setLoopState(true);
	sound.setPitch(1.25f);
	sound.playSound();
	SoundManager::get().addSound(&sound, SOUND_MUSIC);

	Game game;
	game.start();

	TextureManager::unloadAllTextures();

	SoundContext::deleteContext();
	return 0;
}
