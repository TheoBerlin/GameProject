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

void StateManager::updateLogic()
{
	if (!isEmpty())
		this->stack.top()->updateLogic();
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
}

void StateManager::push(State * state)
{
	state->setStateManager(this);

	if (isEmpty() == false) {
		this->stack.top()->end();
	}

	this->stack.push(state);
	state->start();
}

bool StateManager::isEmpty() const
{
	return this->stack.empty();
}
