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
		this->stack.top().first->update(dt);
}

void StateManager::updateLogic()
{
	if (!isEmpty())
		this->stack.top().first->updateLogic();
}

void StateManager::render()
{
	if (!isEmpty())
		this->stack.top().first->render();
}

void StateManager::pop()
{
	std::pair<State*, bool>& top = this->stack.top();
	State* state = top.first;
	state->end();
	if (top.second) {
		delete state;
	}
	this->stack.pop();
}

void StateManager::push(State * state, bool shouldSelfDelete)
{
	if (isEmpty() == false) {
		this->stack.top().first->end();
	}

	this->stack.push(std::pair<State*, bool>(state, shouldSelfDelete));
	state->start();
}

bool StateManager::isEmpty() const
{
	return this->stack.empty();
}
