#pragma once

#include "../Entity/EntityManager.h"

class StateManager;
class State
{
public:
	State();
	virtual ~State();

	void setStateManager(StateManager* stateManager);

	/*
	Calls when state is pushed.
	*/
	virtual void start() {};

	/*
	Calls when state is poped.
	*/
	virtual void end() {};
	virtual void update(const float dt) {};
	virtual void updateLogic() {};
	virtual void render() {};

	/*
	Push state on stack. Can not push in constructor.
	*/
	void pushState(State* state);

	/*
	Pop state from stack. Can not pop in destructor.
	*/
	void popState();

	StateManager& getStateManager();
	EntityManager& getEntityManager();

private:
	StateManager* stateManager;
	EntityManager entityManager;
};