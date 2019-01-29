#include "Game/Game.h"

#include "Engine/Entity/EntityManager.h"
#include <crtdbg.h>

int main() 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Game game;
	game.start();



	return 0;
}
