#include "State.h"

#include "StateManager.h"

State::State()
{
	this->stateManager = nullptr;
}

State::~State()
{
}

void State::setStateManager(StateManager * stateManager)
{
	this->stateManager = stateManager;
}

void State::pushState(State * state)
{
	this->getStateManager().push(state);
}

void State::popState(unsigned popAmount)
{
	this->getStateManager().pop(popAmount);
}

StateManager& State::getStateManager()
{
	return *this->stateManager;
}

GUI & State::getGUI()
{
	return this->gui;
}
