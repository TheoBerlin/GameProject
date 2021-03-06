#pragma once

#include <string>
#include <functional>

class GUI;
class Panel;
class Button;
class StateManager;
struct KeyEvent;

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

	Button* getEditorBtn();

private:
	void activateGUI(Panel* p);

	// Create main menu GUI
	void createMainMenuGUI();
	// Create level select GUI
	void createLevelSelectGUI();
	// Create settings gui
	void createSettingsGUI();

	// Creates a main menu button
	Button* addMainMenuButton(Panel* parent, std::string text, int offset, const std::function<void()>& func);
	// Creates a settings slider
	void addSettingsSlider(Panel* parent, std::string text, int offset, float startFactor, const std::function<void(float)>& func);
	// Creates a settings button
	void addToggleButton(Panel* parent, std::string text, int offset, const std::function<void()>& func);

	// Button callback for mute sound
	void muteSoundCallback();
	// Fullscreen callback
	void fullscreenCallback();
	// Callback to save sound settings
	void saveSoundSettings();

	// Callback to hide editor button
	void hideEditorCallback(KeyEvent* e);

	GUI* gui;
	StateManager* stateManager;

	Panel* currentGUI;
	Panel* mainMenuGUI;
	Panel* levelSelectGUI;
	Panel* settingsGUI;
	Button* editorBtn;

	bool muteSound;
};