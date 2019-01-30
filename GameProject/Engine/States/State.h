#pragma once

class StateManager;
class State
{
public:
	State(StateManager& stateManager);
	virtual ~State();

	virtual void start() {};
	virtual void end() {};
	virtual void update(const float dt) {};
	virtual void updateLogic() {};
	virtual void render() {};

	void pushState(State* state, bool shouldSelfDelete = false);
	void popState();

	StateManager& getStateManager();

private:
	StateManager& stateManager;
};