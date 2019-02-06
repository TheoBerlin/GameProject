#pragma once

#include "../Entity/EntityManager.h"

class StateManager;
class State
{
public:
	State();
	virtual ~State();

	void setStateManager(StateManager* stateManager);

	virtual void start() {};
	virtual void end() {};
	virtual void update(const float dt) {};
	virtual void updateLogic() {};
	virtual void render() {};

	void pushState(State* state);
	void popState();

	StateManager& getStateManager();
	EntityManager& getEntityManager();

private:
	StateManager* stateManager;
	EntityManager entityManager;
};