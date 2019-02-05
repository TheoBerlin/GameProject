#pragma once

#include "../../Engine/States/State.h"

#include "../../Engine/Events/EventBus.h"

class Entity;
class TestState : public State
{
public:
	TestState();
	virtual ~TestState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic() override;
	void render() override;

private:
	void moveCube(KeyEvent* e);

	Entity* cameraEntity;
	Entity* entity;
};