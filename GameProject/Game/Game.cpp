#include "Game.h"

Game::Game()
{
	this->menuState = new MenuState(this->getStateManager());
	this->getStateManager().push(menuState);
}

Game::~Game()
{
	delete this->menuState;
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
