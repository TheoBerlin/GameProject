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

void StateManager::pop(unsigned popAmount)
{
	while (popAmount > 0) {
		State* top = this->stack.top();
		top->end();
		delete top;
		this->stack.pop();

		/*
			Activate gui and run start on the new top state if available
		*/
		if (!this->stack.empty() && popAmount == 1) {
			this->stack.top()->start();
			this->stack.top()->getGUI().setActive(true);
		}

		if (!this->lowerStates.empty()) {
			this->lowerStates.pop();
		}

		popAmount--;
	}

}

void StateManager::push(State * state)
{
	state->setStateManager(this);

	if (isEmpty() == false) {
		this->lowerStates.push(this->stack.top());
		this->lowerStates.top()->end();
		this->lowerStates.top()->getGUI().setActive(false);
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

size_t StateManager::getStackSize()
{
	return this->stack.size();
}
