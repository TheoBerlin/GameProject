#include "MenuState.h"

#include "Engine/States/StateManager.h"
#include "GameState.h"
#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/GUIRenderer.h"
#include "glm/vec4.hpp"
#include "Engine/InputHandler.h"
#include "Utils/Logger.h"

#include "Engine/GUI/FontManager.h"
#include "Engine/GUI/Button.h"
#include "Engine/GUI/ScrollPanel/ScrollPanel.h"


MenuState::MenuState() : State()
{
	// Create panel groups [0] is main menu, [1] is level select
	this->panelGroups.push_back(std::vector<Panel*>());
	this->panelGroups.push_back(std::vector<Panel*>());

	// Default level
	this->selectedLevel = ".";

	// Add fonts for later
	FontManager::addFont("times", "./Game/assets/fonts/times/times.ttf", 16);
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arialbd.ttf", 22);
	FontManager::addFont("arialBig", "./Game/assets/fonts/arial/arialbd.ttf", 36);
	FontManager::addFont("aldo", "./Game/assets/fonts/aldo/aldo.ttf", 40);
	FontManager::addFont("aldoBig", "./Game/assets/fonts/aldo/aldo.ttf", 150);

	this->initMainMenu();
	this->initLevelSelect();

	InputHandler ih(Display::get().getWindowPtr());
}

MenuState::~MenuState()
{
}

void MenuState::start()
{
	// Unlock cursor
	glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void MenuState::end()
{
}

void MenuState::update(const float dt)
{
	static float time = 0.0f;
	time += dt;
	if (time > 1.0f)
	{
		if(this->panel)
			this->panel->updateText(0, "FPS: " + std::to_string((int)(1.0f / dt)));
		time = 0.0f;
	}
}

void MenuState::updateLogic(const float dt)
{
}

void MenuState::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	Display& display = Display::get();
	
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	guiRenderer.draw(this->getGUI());
}

void MenuState::initLevelSelect()
{
	std::vector<std::experimental::filesystem::path> levels;
	loadLevelPaths("./Game/Level/Levels", levels);

	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	// Create scroll list for level selection
	unsigned width = 200;
	unsigned height = 300;
	ScrollPanel* scrollPanel = new ScrollPanel(width, height);
	scrollPanel->setOption(GUI::FLOAT_DOWN, 10);
	scrollPanel->setOption(GUI::CENTER_X);
	scrollPanel->setColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.f));

	for (auto entry : levels)
	{
		scrollPanel->addItem([this, entry](void) {
			this->selectedLevel = entry.string();
		}, entry.filename().replace_extension("").string());
	}

	if (!levels.empty())
	{
		scrollPanel->setActiveButton(0);
		this->selectedLevel = levels[0].string();
	}

	this->panelGroups[1].push_back(scrollPanel);
	this->getGUI().addPanel(scrollPanel);

	// Create select/play button
	Button* playBtn = new Button();
	playBtn->setOption(GUI::SCALE_TO_TEXT_X, 5);
	playBtn->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	playBtn->setOption(GUI::CENTER_X);
	playBtn->setOption(GUI::CENTER_Y, 70);
	playBtn->setOption(GUI::TEXT_CENTER_X);
	playBtn->setOption(GUI::TEXT_CENTER_Y);
	glm::vec4 buttonColor(0.4, 0.4, 0.4, 1.0);
	playBtn->setHoverColor(buttonColor * 1.2f);
	playBtn->setNormalColor(buttonColor);
	playBtn->setPressedColor(buttonColor * 0.8f);
	playBtn->addText("Play", "aldo", glm::vec4(1.0f));
	playBtn->setCallback([this](void) {
		this->pushState(new GameState(this->selectedLevel));
	});
	this->panelGroups[1].push_back(playBtn);
	this->getGUI().addPanel(playBtn);

	// Create back button
	Button* backBtn = new Button();
	backBtn->setOption(GUI::SCALE_TO_TEXT_X, 5);
	backBtn->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	backBtn->setOption(GUI::CENTER_X);
	backBtn->setOption(GUI::CENTER_Y);
	backBtn->setOption(GUI::TEXT_CENTER_X);
	backBtn->setOption(GUI::TEXT_CENTER_Y);
	backBtn->setHoverColor(buttonColor * 1.2f);
	backBtn->setNormalColor(buttonColor);
	backBtn->setPressedColor(buttonColor * 0.8f);
	backBtn->addText("Back", "aldo", glm::vec4(1.0f));
	backBtn->setCallback([this](void) {
		for (auto p : this->panelGroups[1])
			p->hide();
		for (auto p : this->panelGroups[0])
			p->show();
	});
	this->panelGroups[1].push_back(backBtn);
	this->getGUI().addPanel(backBtn);

	// Hide these panels in the beginning
	for (auto p : this->panelGroups[1])
		p->hide();
}

void MenuState::loadLevelPaths(std::string dir, std::vector<std::experimental::filesystem::path>& paths)
{
	for (const auto& entry : std::experimental::filesystem::directory_iterator(dir))
		paths.push_back(entry.path());
}

void MenuState::initMainMenu()
{
	// Colors for creation
	glm::vec4 textColor = { 0.9f, 0.9f, 0.9f, 1.0f };
	glm::vec4 backgroundColor = { 0.1f, 0.1f, 0.1f, 0.99f };
	glm::vec4 hoverColor = { 0.5f, 0.0f, 0.5f, 1.0f };
	glm::vec4 pressColor = { 0.3f, 0.0f, 0.3f, 1.0f };

	// Get GUI
	GUI& gui = this->getGUI();

	// Create title
	Panel* titlePnl = new Panel();
	titlePnl->addText("gameNname", "aldoBig", textColor);
	titlePnl->setOption(GUI::FLOAT_UP, 40);
	titlePnl->setOption(GUI::CENTER_X);
	titlePnl->setOption(GUI::SCALE_TO_TEXT_X);
	titlePnl->setColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	this->panelGroups[0].push_back(titlePnl);
	gui.addPanel(titlePnl);

	// Create play button
	this->button = new Button();
	this->button->setOption(GUI::SCALE_TO_TEXT_X, 5);
	this->button->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	this->button->setOption(GUI::CENTER_X);
	this->button->setOption(GUI::CENTER_Y);
	this->button->setOption(GUI::TEXT_CENTER_X);
	this->button->setOption(GUI::TEXT_CENTER_Y);
	glm::vec4 buttonColor(0.4, 0.4, 0.4, 1.0);
	this->button->setHoverColor(buttonColor * 1.2f);
	this->button->setNormalColor(buttonColor);
	this->button->setPressedColor(buttonColor * 0.8f);
	this->button->addText("Select level", "aldo", glm::vec4(1.0f));
	this->button->setCallback([this](void) {
		for (auto p : this->panelGroups[0])
			p->hide();
		for (auto p : this->panelGroups[1])
			p->show();
	});
	this->panelGroups[0].push_back(this->button);
	gui.addPanel(this->button);
}
