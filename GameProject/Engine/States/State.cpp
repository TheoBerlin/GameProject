#include "State.h"

#include "StateManager.h"

State::State(StateManager& stateManager) : stateManager(stateManager)
{
}

State::~State()
{
}

void State::pushState(State * state, bool shouldSelfDelete)
{
	this->getStateManager().push(state, shouldSelfDelete);
}

void State::popState()
{
	this->getStateManager().pop();
}

StateManager& State::getStateManager()
{
	return this->stateManager;
}
