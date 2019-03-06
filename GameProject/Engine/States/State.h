#pragma once

#include "../Entity/EntityManager.h"
#include "../GUI/GUI.h"

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
	virtual void updateLogic(const float dt) {};
	virtual void render() {};

	/*
	Push state on stack. Can not push in constructor.
	*/
	void pushState(State* state);

	/*
	Pop state from stack. Can not pop in destructor.
	*/
	void popState(unsigned popAmount = 1);

	StateManager& getStateManager();
	EntityManager& getEntityManager();
	GUI& getGUI();

private:
	StateManager* stateManager;
	EntityManager entityManager;
	GUI gui;
};