#include "MenuGUI.h"

#include "Engine/GUI/Panel.h"
#include "Engine/GUI/Button.h"
#include "Engine/GUI/SliderPanel.h"
#include "Engine/GUI/GUIColors.h"

#include "Engine/Sound/SoundManager.h"
#include "Utils/Settings.h"
#include "Engine/States/StateManager.h"
#include "Game/States/EditorState.h"

#include "Engine/Events/EventBus.h"
#include "Engine/Rendering/Display.h"

MenuGUI::MenuGUI()
{
	this->currentGUI = nullptr;
	this->muteSound = false;

	EventBus::get().subscribe(this, &MenuGUI::hideEditorCallback);
}

MenuGUI::~MenuGUI()
{
	EventBus::get().unsubscribe(this, &MenuGUI::hideEditorCallback);
}

void MenuGUI::init(GUI * gui, StateManager* stateManager)
{
	this->gui = gui;
	this->stateManager = stateManager;

	createMainMenuGUI();
	createLevelSelectGUI();
	createSettingsGUI();

	activateGUI(this->mainMenuGUI);
}

void MenuGUI::switchGUI(MENU option)
{
	switch (option)
	{
	case MENU::MAIN:
		activateGUI(this->mainMenuGUI);
		break;
	case MENU::LEVEL_SELECT:
		activateGUI(this->levelSelectGUI);
		break;
	case MENU::SETTINGS:
		activateGUI(this->settingsGUI);
		break;
	}
}

void MenuGUI::changePanelPointer(MENU menu, Panel * newPanel)
{
	switch (menu)
	{
	case MENU::MAIN:
		this->mainMenuGUI = newPanel;
		break;
	case MENU::LEVEL_SELECT:
		this->levelSelectGUI = newPanel;
		break;
	case MENU::SETTINGS:
		this->settingsGUI = newPanel;
		break;
	default:
		return;
	}
	newPanel->hide();
}

void MenuGUI::setStateManager(StateManager * stateManager)
{
	this->stateManager = stateManager;
}

void MenuGUI::activateGUI(Panel * p)
{
	if (this->currentGUI)
		this->currentGUI->hide();
	this->currentGUI = p;
	this->currentGUI->show();
}

void MenuGUI::createMainMenuGUI()
{
	// Create main menu container panel
	this->mainMenuGUI = new Panel();
	this->mainMenuGUI->setOption(GUI::FIT_X);
	this->mainMenuGUI->setOption(GUI::FIT_Y);
	this->mainMenuGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->mainMenuGUI);

	// Create title
	Panel* titlePnl = new Panel();
	titlePnl->addText("gameNname", "aldoBig", TEXT_COLOR);
	titlePnl->setOption(GUI::FLOAT_UP, 40);
	titlePnl->setOption(GUI::CENTER_X);
	titlePnl->setOption(GUI::SCALE_TO_TEXT_X);
	titlePnl->setColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	this->mainMenuGUI->addChild(titlePnl);

	addMainMenuButton(this->mainMenuGUI, "SELECT LEVEL", 0, [this]() { this->activateGUI(this->levelSelectGUI); });
	addMainMenuButton(this->mainMenuGUI, "SETTINGS", -100, [this]() { this->activateGUI(this->settingsGUI); });
	addMainMenuButton(this->mainMenuGUI, "EXIT", -200, [this]() { this->stateManager->pop(); });
	this->editorBtn = addMainMenuButton(this->mainMenuGUI, "EDITOR", -300, [this]() { this->stateManager->push(new EditorState()); });
	this->editorBtn->hide();

	this->mainMenuGUI->hide();
}

void MenuGUI::createLevelSelectGUI()
{
	// Create level select container panel
	this->levelSelectGUI = new Panel();
	this->levelSelectGUI->setOption(GUI::FIT_X);
	this->levelSelectGUI->setOption(GUI::FIT_Y);
	this->levelSelectGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->levelSelectGUI);

	this->levelSelectGUI->hide();
}

void MenuGUI::createSettingsGUI()
{
	// Create settings container panel
	this->settingsGUI = new Panel();
	this->settingsGUI->setOption(GUI::FIT_X);
	this->settingsGUI->setOption(GUI::FIT_Y);
	this->settingsGUI->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->gui->addPanel(this->settingsGUI);

	addSettingsSlider(this->settingsGUI, "MASTER VOLUME", 0, SoundManager::get().getMasterVolume(), [this](float v) {SoundManager::get().setMasterVolume(v);
		if (this->muteSound && SoundManager::get().getMasterVolume() > 0.f || !this->muteSound && SoundManager::get().getMasterVolume() == 0.f)
			this->muteSoundCallback();
	});
	addSettingsSlider(this->settingsGUI, "MUSIC VOLUME", -75, SoundManager::get().getMusicVolume(), [this](float v) {SoundManager::get().setMusicVolume(v); });
	addSettingsSlider(this->settingsGUI, "AMBIENT VOLUME", -150, SoundManager::get().getAmbientVolume(), [this](float v) {SoundManager::get().setAmbientVolume(v); });
	addSettingsSlider(this->settingsGUI, "EFFECTS VOLUME", -225, SoundManager::get().getEffectsVolume(), [this](float v) {SoundManager::get().setEffectsVolume(v); });
	addSettingsSlider(this->settingsGUI, "MISC VOLUME", -300, SoundManager::get().getMiscVolume(), [this](float v) {SoundManager::get().setMiscVolume(v); });
	addToggleButton(this->settingsGUI, "MUTE SOUND", -375, [this]() { this->muteSoundCallback(); });
	addSettingsSlider(this->settingsGUI, "MOUSE SENSITIVITY", -450, Settings::get().getMouseSensitivity(), [this](float v) { Settings::get().setMouseSensitivity(v); });
	addToggleButton(this->settingsGUI, "FULLSCREEN", -525, [this]() { this->fullscreenCallback(); });

	// Create text 
	Panel* txt = new Panel();
	txt->setOption(GUI::CENTER_X, -200);
	txt->setOption(GUI::CENTER_Y, -575 + 300);
	txt->setOption(GUI::SCALE_TO_TEXT_X, 5);
	txt->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	txt->setColor(PANEL_BACKGROUND_COLOR);
	txt->addText("(REQUIRES RESTART)", "aldo", TEXT_COLOR);
	this->settingsGUI->addChild(txt);

	// Add back button
	Button* backBtn = new Button();
	backBtn->setOption(GUI::SCALE_TO_TEXT_X, 5);
	backBtn->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	backBtn->setOption(GUI::FLOAT_LEFT, 50);
	backBtn->setOption(GUI::FLOAT_DOWN, 50);
	backBtn->setOption(GUI::TEXT_CENTER_X);
	backBtn->setOption(GUI::TEXT_CENTER_Y);
	backBtn->setHoverColor(BUTTON_HOVER_COLOR);
	backBtn->setNormalColor(BUTTON_NORMAL_COLOR);
	backBtn->setPressedColor(BUTTON_PRESS_COLOR);
	backBtn->addText("BACK", "aldo", glm::vec4(1.0f));
	backBtn->setCallback([this](void) { this->activateGUI(this->mainMenuGUI); this->saveSoundSettings(); });
	this->settingsGUI->addChild(backBtn);

	if (SoundManager::get().getMasterVolume() == 0.f)
		muteSoundCallback();

	this->settingsGUI->hide();
}

Button* MenuGUI::addMainMenuButton(Panel* parent, std::string text, int offset, const std::function<void()>& func)
{
	// Create play button
	Button* btn = new Button();
	btn->setOption(GUI::SCALE_TO_TEXT_X, 5);
	btn->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	btn->setOption(GUI::CENTER_X);
	btn->setOption(GUI::CENTER_Y, offset);
	btn->setOption(GUI::TEXT_CENTER_X);
	btn->setOption(GUI::TEXT_CENTER_Y);
	btn->setHoverColor(BUTTON_HOVER_COLOR);
	btn->setNormalColor(BUTTON_NORMAL_COLOR);
	btn->setPressedColor(BUTTON_PRESS_COLOR);
	btn->addText(text, "aldo", glm::vec4(1.0f));
	btn->setCallback(func);
	parent->addChild(btn);

	return btn;
}

void MenuGUI::addSettingsSlider(Panel * parent, std::string text, int offset, float startFactor, const std::function<void(float)>& func)
{
	// Create text 
	Panel* txt = new Panel();
	txt->setOption(GUI::CENTER_X, -200);
	txt->setOption(GUI::CENTER_Y, offset + 300);
	txt->setOption(GUI::SCALE_TO_TEXT_X, 5);
	txt->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	txt->setColor(PANEL_BACKGROUND_COLOR);
	txt->addText(text, "aldo", TEXT_COLOR);
	parent->addChild(txt);

	// Create slider 
	SliderPanel* slider = new SliderPanel();
	slider->setOption(GUI::CENTER_X, 100);
	slider->setOption(GUI::CENTER_Y, offset + 300);
	slider->setSliderCallback(func);
	slider->setSliderFactor(startFactor);
	parent->addChild(slider);
}

void MenuGUI::addToggleButton(Panel * parent, std::string text, int offset, const std::function<void()>& func)
{
	// Create text 
	Panel* txt = new Panel();
	txt->setOption(GUI::CENTER_X, -200);
	txt->setOption(GUI::CENTER_Y, offset + 300);
	txt->setOption(GUI::SCALE_TO_TEXT_X, 5);
	txt->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	txt->setColor(PANEL_BACKGROUND_COLOR);
	txt->addText(text, "aldo", TEXT_COLOR);
	parent->addChild(txt);

	// Create button
	Button* btn = new Button();
	btn->setOption(GUI::CENTER_X, 100);
	btn->setOption(GUI::CENTER_Y, offset + 300);
	btn->setSize({ 50, 50, });
	btn->setNormalColor(Settings::get().getFullscreen() ? BUTTON_PRESS_COLOR : BUTTON_NORMAL_COLOR);
	btn->setHoverColor(BUTTON_HOVER_COLOR);
	btn->setPressedColor(BUTTON_PRESS_COLOR);
	btn->setCallback(func);
	parent->addChild(btn);
}

void MenuGUI::muteSoundCallback()
{
	this->muteSound = !this->muteSound;
	SoundManager::get().setMasterVolume(this->muteSound ? 0.f : 0.5f);
	SliderPanel* sp = dynamic_cast<SliderPanel*>(this->settingsGUI->getChildren()[1]);
	sp->setSliderFactor(this->muteSound ? 0.f : 0.5f);
	Button* btn = dynamic_cast<Button*>(this->settingsGUI->getChildren()[11]);
	btn->setNormalColor(this->muteSound ? BUTTON_PRESS_COLOR : BUTTON_NORMAL_COLOR);
}

void MenuGUI::fullscreenCallback()
{
	Settings::get().setFullscreen(!Settings::get().getFullscreen());
	Button* btn = dynamic_cast<Button*>(this->settingsGUI->getChildren()[15]);
	btn->setNormalColor(Settings::get().getFullscreen() ? BUTTON_PRESS_COLOR : BUTTON_NORMAL_COLOR);
}

void MenuGUI::saveSoundSettings()
{
	Settings::get().writeFile();
}

void MenuGUI::hideEditorCallback(KeyEvent * e)
{
	if (e->action == GLFW_PRESS && e->key == GLFW_KEY_F11)
	{
		if (this->editorBtn->isShown())
			this->editorBtn->hide();
		else
			this->editorBtn->show();
	}
}
