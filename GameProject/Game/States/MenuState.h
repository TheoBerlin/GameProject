#pragma once

#include <Engine/States/State.h>
#include <Engine/GUI/Text.h>
#include <Engine/GUI/Panel.h>
#include <Engine/GUI/Button.h>

#include <filesystem>

#include <Game/GameLogic/LevelPreviewer.h>
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

	void updateLevelPreview(const std::string& levelName);

	const Level& getLevel() const;

private:
	// Loads levels name from folder to show in scroll panel
	void loadLevelPaths(std::string dir, std::vector<std::experimental::filesystem::path>& paths);
	// Callback for scroll panel button press to update info panel
	void updateLevelInfoPanel();
	// Update score when event is published
	void updateScore(UpdateScoreEvent * evnt);
	// Init main menu GUI panels
	void initMainMenu();
	// Init level select GUI panels
	void initLevelSelect();

	std::string selectedLevel;

	LevelParser levelParser;
	std::vector<std::string> levelInfo;
	std::vector<std::vector<Panel*>> panelGroups;

	Panel* previewPnl;

	LevelPreviewer* levelPreviewer;
};