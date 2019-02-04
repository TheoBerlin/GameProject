#pragma once

#include "../../Engine/States/State.h"

class Entity;
class MenuState : public State
{
public:
	MenuState();
	virtual ~MenuState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic() override;
	void render() override;

private:
	Entity* entity;
};