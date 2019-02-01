#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Game/Game.h"
#include "Engine/AssetManagement/ModelLoader.h"

#include "OpenAL/al.h"
#include "OpenAL/alc.h"

#include "ft2build.h" //DO NOT MOVE HEADER FILE FROM ROOT DIRECTORY OF INCLUDE
#include FT_FREETYPE_H  

#define STB_IMAGE_IMPLEMENTATION
#include "../../Utils/stb_image.h" //Single library for img loader

#define NUM_BUFFERS 3
#define BUFFER_SIZE 4096

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();

	alGetError();

	ALCdevice * deviceAL = alcOpenDevice(NULL);

	if (deviceAL == NULL)
	{
		LOG_ERROR("Failed to init OpenAL device.");
	}

	ALCcontext * contextAL = alcCreateContext(deviceAL, NULL);

	Game game;
	game.start();

	Logger::destroy();

	ModelLoader::unloadModels();
	TextureManager::unloadTextures();

	return 0;
}
