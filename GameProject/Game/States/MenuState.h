#pragma once

#include <Engine/States/State.h>
#include <Engine/GUI/Text.h>
#include <Engine/GUI/Panel.h>
#include <Engine/GUI/Button.h>

#include "Game/Level/LevelParser.h"

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
	void initMainMenu();
	void initLevelSelect();


	std::string selectedLevel;

	LevelParser levelParser;
	std::vector<std::vector<Panel*>> panelGroups;

	Panel* panel;
	Button* button;
};
