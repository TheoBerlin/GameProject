#pragma once

#include <string>
#include <functional>

class GUI;
class Panel;
class StateManager;

enum MENU
{
	MAIN = 0,
	LEVEL_SELECT,
	SETTINGS
};

class MenuGUI
{
public:
	MenuGUI();
	~MenuGUI();

	void init(GUI* gui, StateManager* stateManager);

	void switchGUI(MENU option);

	// Change panel pointer of a menu, can therefore add existing panels instead of creating new
	void changePanelPointer(MENU menu, Panel* newPanel);

	void setStateManager(StateManager* stateManager);

private:
	void activateGUI(Panel* p);

	// Create main menu GUI
	void createMainMenuGUI();
	// Create level select GUI
	void createLevelSelectGUI();
	// Create settings gui
	void createSettingsGUI();

	// Creates a main menu button
	void addMainMenuButton(Panel* parent, std::string text, int offset, const std::function<void()>& func);
	// Creates a settings slider
	void addSettingsSlider(Panel* parent, std::string text, int offset, float startFactor, const std::function<void(float)>& func);
	// Creates a settings button
	void addToggleButton(Panel* parent, std::string text, int offset, const std::function<void()>& func);

	// Button callback for mute sound
	void muteSoundCallback();
	// Callback to save sound settings
	void saveSoundSettings();

	GUI* gui;
	StateManager* stateManager;

	Panel* currentGUI;
	Panel* mainMenuGUI;
	Panel* levelSelectGUI;
	Panel* settingsGUI;

	bool muteSound;
};