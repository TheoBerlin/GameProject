#include "MenuState.h"

#include "../../Engine/States/StateManager.h"
#include "TestState.h"

MenuState::MenuState() : State()
{
	
}

MenuState::~MenuState()
{
}

void MenuState::start()
{
	this->pushState(new TestState());
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
