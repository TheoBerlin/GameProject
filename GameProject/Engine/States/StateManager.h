#pragma once

#include <stack>

#include "State.h"

class StateManager
{
public:
	StateManager();
	virtual ~StateManager();

	void update(float dt);
	void updateLogic();
	void render();

	State* pop();
	void push(State* state);

private:
	std::stack<State*> stack;
};