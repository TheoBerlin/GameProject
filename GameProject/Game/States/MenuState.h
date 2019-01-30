#pragma once

#include "../../Engine/States/State.h"

class MenuState : public State
{
public:
	MenuState(StateManager& stateManager);
	virtual ~MenuState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic() override;
	void render() override;

private:
};