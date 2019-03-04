#pragma once

#include <stack>

#include "State.h"

class StateManager
{
public:
	StateManager();
	virtual ~StateManager();

	/*
	Calls update on top state.
	Arguments:
		dt: Deltatime of current frame.
	*/
	void update(const float dt);

	/*
	Calls updatedLogic on top state.
	*/
	void updateLogic(const float dt);

	/*
	Calls render on top state.
	*/
	void render();

	/* 
	Remove top state from stack and call its "end" function. Will destroy state if is marked as such.
	*/
	void pop(unsigned popAmount = 1);

	/*
	Add a state on top of stack and call its "start" function.
	Arguments:
		state:				The state to be added.
	*/
	void push(State* state);

	/*
	Check if stack is empty.
	Return:
		True if empty, false otherwise.
	*/
	bool isEmpty() const;

	/*
	Returns the state which is one lower than the top.
	If the top state is the only state, then this will return a nullptr.
	*/
	State* getLowerState();

	/*
	Returns the state which is one lower than the top.
	If the top state is the only state, then this will return a nullptr.
	*/
	size_t getStackSize();

private:
	// Stack which holds all the states and a bool which tells if it should be deleted.
	std::stack<State*> stack;
	// Holds all the states lower than the top. If you want the previous state of the top state in the stack, then use lowerStates.top().
	std::stack<State*> lowerStates;
};