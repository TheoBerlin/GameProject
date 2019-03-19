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
#include "Engine/GUI/GUIColors.h"
#include "Game/States/EditorState.h"
#include "Engine/GUI/SliderPanel.h"

MenuState::MenuState()
	:State(),
	levelPreviewer(nullptr)
{
	// Default level
	this->selectedLevel = ".";

	// Add fonts for later
	FontManager::addFont("times", "./Game/assets/fonts/times/times.ttf", 16);
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arialbd.ttf", 22);
	FontManager::addFont("arialBig", "./Game/assets/fonts/arial/arialbd.ttf", 36);
	FontManager::addFont("aldo", "./Game/assets/fonts/aldo/aldo.ttf", 40);
	FontManager::addFont("aldoSmall", "./Game/assets/fonts/aldo/aldo.ttf", 20);
	FontManager::addFont("aldoBig", "./Game/assets/fonts/aldo/aldo.ttf", 150);

	menuGUI.init(&this->getGUI(), &this->getStateManager());

	Button* editorBtn = menuGUI.getEditorBtn();

	editorBtn->setCallback([this](){
		if (this->levelPreviewer) {
			delete levelPreviewer;
			this->levelPreviewer = nullptr;
		}

		this->entityManager.removeEntities();

		this->pushState(new EditorState());
	});

	this->initLevelSelect();

	InputHandler ih(Display::get().getWindowPtr());
	EventBus::get().subscribe(this, &MenuState::updateScore);
}

MenuState::~MenuState()
{
	if (levelPreviewer) {
		delete levelPreviewer;

		levelPreviewer = nullptr;
	}

	TextureManager::unloadAllTextures();
}

void MenuState::start()
{
	this->updateLevelPreview(this->selectedLevel);

	menuGUI.setStateManager(&this->getStateManager());

	// Unlock cursor
	glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MenuState::end()
{
}

void MenuState::update(const float dt)
{
	levelPreviewer->update(dt);
}

void MenuState::updateLogic(const float dt)
{
}

void MenuState::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	levelPreviewer->render();

	glDisable(GL_DEPTH_TEST);
	Display& display = Display::get();

	GUIRenderer& guiRenderer = display.getGUIRenderer();

	guiRenderer.draw(this->getGUI());
}

void MenuState::updateLevelPreview(const std::string& levelName)
{
	if (this->levelPreviewer) {
		delete levelPreviewer;
	}

	this->entityManager.removeEntities();

	this->levelPreviewer = new LevelPreviewer(&this->entityManager);

	this->levelPreviewer->setLevel(levelName);
}

void MenuState::stopPreviewReplay()
{
	this->levelPreviewer->stopReplaying();
}

Level& MenuState::getLevel()
{
	return this->levelPreviewer->getLevel();
}

void MenuState::loadLevelPaths(std::string dir, std::vector<std::experimental::filesystem::path>& paths)
{
	for (const auto& entry : std::experimental::filesystem::directory_iterator(dir))
		paths.push_back(entry.path());
}

void MenuState::updateLevelInfoPanel()
{
	this->levelParser.readLevelInfo(this->selectedLevel, this->levelInfo);
	std::vector<Panel*> children = this->previewPnl->getChildren();
	children[0]->updateText(0, this->selectedLevel.substr(this->selectedLevel.rfind("\\") + 1, this->selectedLevel.rfind(".") - this->selectedLevel.rfind("\\") - 1));
	children[1]->updateText(0, this->levelInfo[0]);
	children[2]->updateText(0, this->levelInfo[1]);
	children[3]->updateText(0, this->levelInfo[2]);
}

void MenuState::initLevelSelect()
{
	std::vector<std::experimental::filesystem::path> levels;
	loadLevelPaths("./Game/Level/Levels", levels);

	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	// Create contain panel
	Panel* container = new Panel();
	container->setOption(GUI::FIT_X);
	container->setOption(GUI::FIT_Y);
	container->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->getGUI().addPanel(container);

	// Create panel for scroll list and its two buttons
	Panel* selectPnl = new Panel();
	selectPnl->setOption(GUI::FLOAT_LEFT);
	selectPnl->setSize({ 300, display.getHeight() });
	selectPnl->setColor(PANEL_BACKGROUND_COLOR);
	container->addChild(selectPnl);

	// Create scroll list for level selection
	ScrollPanel* scrollPanel = new ScrollPanel(200, 300);
	scrollPanel->setOption(GUI::FLOAT_DOWN, 10);
	scrollPanel->setOption(GUI::CENTER_X);
	scrollPanel->setColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.f));

	// Create scroll panel buttons for level and add callbacks for them
	for (auto entry : levels)
	{
		if (entry.has_extension())
		{
			scrollPanel->addItem([this, entry](void) {
				this->selectedLevel = entry.string();
				this->updateLevelInfoPanel();
				this->updateLevelPreview(this->selectedLevel);
			}, entry.filename().replace_extension("").string());
		}
	}

	// Set the first level to be the selected one
	if (!levels.empty())
	{
		scrollPanel->setActiveButton(0);
		this->selectedLevel = levels[0].string();
	}

	selectPnl->addChild(scrollPanel);

	// Create select/play button
	Button* playBtn = new Button();
	playBtn->setOption(GUI::SCALE_TO_TEXT_X, 5);
	playBtn->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	playBtn->setOption(GUI::CENTER_Y, 70);
	playBtn->setOption(GUI::CENTER_X);
	playBtn->setOption(GUI::TEXT_CENTER_X);
	playBtn->setOption(GUI::TEXT_CENTER_Y);
	playBtn->setHoverColor(BUTTON_HOVER_COLOR);
	playBtn->setNormalColor(BUTTON_NORMAL_COLOR);
	playBtn->setPressedColor(BUTTON_PRESS_COLOR);
	playBtn->addText("Play", "aldo", glm::vec4(1.0f));
	playBtn->setCallback([this](void) {
		this->stopPreviewReplay();
		this->pushState(new GameState(this->getLevel()));
	});
	selectPnl->addChild(playBtn);

	// Create back button
	Button* backBtn = new Button();
	backBtn->setOption(GUI::SCALE_TO_TEXT_X, 5);
	backBtn->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	backBtn->setOption(GUI::CENTER_X);
	backBtn->setOption(GUI::CENTER_Y);
	backBtn->setOption(GUI::TEXT_CENTER_X);
	backBtn->setOption(GUI::TEXT_CENTER_Y);
	backBtn->setHoverColor(BUTTON_HOVER_COLOR);
	backBtn->setNormalColor(BUTTON_NORMAL_COLOR);
	backBtn->setPressedColor(BUTTON_PRESS_COLOR);
	backBtn->addText("Back", "aldo", glm::vec4(1.0f));
	backBtn->setCallback([this](void) {
		menuGUI.switchGUI(MENU::MAIN);
	});
	selectPnl->addChild(backBtn);

	// Create level preview panel
	Panel* previewPnl = new Panel();
	this->previewPnl = previewPnl;
	previewPnl->setOption(GUI::FLOAT_UP, 40);
	previewPnl->setOption(GUI::CENTER_X);
	previewPnl->setOption(GUI::CENTER_Y);
	previewPnl->setSize({ 500, 500 });
	previewPnl->setColor(PANEL_BACKGROUND_COLOR);
	container->addChild(previewPnl);

	// Create name for map panel
	Panel* namePnl = new Panel();
	namePnl->setColor({ 0.f, 0.f, 0.f, 0.f });
	namePnl->addText(this->selectedLevel.substr(this->selectedLevel.rfind("\\") + 1, this->selectedLevel.rfind(".") - this->selectedLevel.rfind("\\") - 1), "aldo", TEXT_COLOR);
	namePnl->setOption(GUI::SCALE_TO_TEXT_X);
	namePnl->setOption(GUI::SCALE_TO_TEXT_Y);
	namePnl->setOption(GUI::CENTER_X);
	namePnl->setOption(GUI::FLOAT_UP, 20);
	previewPnl->addChild(namePnl);

	// Create score for map panel
	this->levelParser.readLevelInfo(this->selectedLevel, this->levelInfo);
	Panel* scorePnl = new Panel();
	scorePnl->setColor({ 0.f, 0.f, 0.f, 0.f });
	scorePnl->addText(this->levelInfo[0], "aldo", TEXT_COLOR);
	scorePnl->setOption(GUI::SCALE_TO_TEXT_X);
	scorePnl->setOption(GUI::SCALE_TO_TEXT_Y);
	scorePnl->setOption(GUI::FLOAT_LEFT, 20);
	scorePnl->setOption(GUI::FLOAT_UP, 80);
	previewPnl->addChild(scorePnl);

	// Create time for map panel
	Panel* timePnl = new Panel();
	timePnl->setColor({ 0.f, 0.f, 0.f, 0.f });
	timePnl->addText(this->levelInfo[1], "aldo", TEXT_COLOR);
	timePnl->setOption(GUI::SCALE_TO_TEXT_X);
	timePnl->setOption(GUI::SCALE_TO_TEXT_Y);
	timePnl->setOption(GUI::FLOAT_LEFT, 20);
	timePnl->setOption(GUI::FLOAT_UP, 140);
	previewPnl->addChild(timePnl);

	Panel* hiScorePnl = new Panel();
	hiScorePnl->setColor({ 0.f, 0.f, 0.f, 0.f });
	hiScorePnl->addText(this->levelInfo[2], "aldo", TEXT_COLOR);
	hiScorePnl->setOption(GUI::SCALE_TO_TEXT_X);
	hiScorePnl->setOption(GUI::SCALE_TO_TEXT_Y);
	hiScorePnl->setOption(GUI::FLOAT_LEFT, 20);
	hiScorePnl->setOption(GUI::FLOAT_UP, 200);
	previewPnl->addChild(hiScorePnl);
	
	menuGUI.changePanelPointer(MENU::LEVEL_SELECT, container);
}

void MenuState::updateScore(NewHighscoreEvent * evnt)
{
	std::vector<Panel*> children = this->previewPnl->getChildren();
	children[3]->updateText(0, "Highscore:" + std::to_string(evnt->highscore));
}
