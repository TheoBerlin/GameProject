#include "StateManager.h"

StateManager::StateManager()
{
}

StateManager::~StateManager()
{
}

void StateManager::update(float dt)
{
	this->stack.top()->update(dt);
}

void StateManager::updateLogic()
{
	this->stack.top()->updateLogic();
}

void StateManager::render()
{
	this->stack.top()->render();
}

State* StateManager::pop()
{
	State* state = this->stack.top();
	this->stack.pop();
	return state;
}

void StateManager::push(State * state)
{
	this->stack.push(state);
}
