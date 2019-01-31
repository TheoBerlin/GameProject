#include <crtdbg.h>

#include "Utils/Logger.h"
#include "Game/Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../Utils/stb_image.h" //Single library for img loader

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Logger::init();

	Game game;
	game.start();

	Logger::destroy();
	return 0;
}
