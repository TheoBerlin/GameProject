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

void State::popState()
{
	this->getStateManager().pop();
}

StateManager& State::getStateManager()
{
	return *this->stateManager;
}

EntityManager & State::getEntityManager()
{
	return this->entityManager;
}

GUI & State::getGUI()
{
	return this->gui;
}
