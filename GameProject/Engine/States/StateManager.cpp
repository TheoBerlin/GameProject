#include "StateManager.h"

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
	while (isEmpty() == false) {
		pop();
	}
}

void StateManager::update(const float dt)
{
	if(!isEmpty())
		this->stack.top()->update(dt);
}

void StateManager::updateLogic(const float dt)
{
	if (!isEmpty())
		this->stack.top()->updateLogic(dt);
}

void StateManager::render()
{
	if (!isEmpty())
		this->stack.top()->render();
}

void StateManager::pop()
{
	State* top = this->stack.top();
	State* state = top;
	state->end();
	delete state;
	this->stack.pop();

	if (this->lowerStates.empty() == false) {
		this->lowerStates.pop();
	}
}

void StateManager::push(State * state)
{
	state->setStateManager(this);

	if (isEmpty() == false) {
		this->lowerStates.push(this->stack.top());
		this->lowerStates.top()->end();
	}

	this->stack.push(state);
	state->start();
}

bool StateManager::isEmpty() const
{
	return this->stack.empty();
}

State * StateManager::getLowerState()
{
	if (this->lowerStates.empty())
		return nullptr;
	return this->lowerStates.top();
}
