#include "Game.h"

Game::Game()
{
	this->getStateManager().push(new MenuState());
}

Game::~Game()
{
}

void Game::onUpdate(float dt)
{
}

void Game::onUpdateLogic()
{
}

void Game::onRender()
{
}
