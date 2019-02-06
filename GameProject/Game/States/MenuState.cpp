#include "MenuState.h"

#include "../../Engine/States/StateManager.h"
#include "GameState.h"
#include "TestState.h"

MenuState::MenuState() : State()
{
	
}

MenuState::~MenuState()
{
}

void MenuState::start()
{
	this->pushState(new GameState());
}

void MenuState::end()
{
}

void MenuState::update(const float dt)
{
}

void MenuState::updateLogic()
{
}

void MenuState::render()
{
}
