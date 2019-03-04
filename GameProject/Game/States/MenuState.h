#pragma once

#include <Engine/States/State.h>
#include <Engine/GUI/Text.h>
#include <Engine/GUI/Panel.h>
#include <Engine/GUI/Button.h>

class MenuState : public State
{
public:
	MenuState();
	virtual ~MenuState();

	void start() override;
	void end() override;
	void update(const float dt) override;
	void updateLogic(const float dt) override;
	void render() override;

private:
	void initPanelLayout();
	void initLevelSelectLayout();

	std::string selectedLevel;

	Panel* panel;
	Button* button;
};
