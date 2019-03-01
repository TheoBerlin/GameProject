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
	//Default level
	this->selectedLevel = "./Game/Level/level.json";

	FontManager::addFont("times", "./Game/assets/fonts/times/times.ttf", 16);
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arialbd.ttf", 22);
	FontManager::addFont("arialBig", "./Game/assets/fonts/arial/arialbd.ttf", 36);
	FontManager::addFont("aldo", "./Game/assets/fonts/aldo/aldo.ttf", 40);
	this->font = FontManager::getFont("arial");
	test.setText("------", this->font);
	test.setColor({1.0f, 1.0f, 1.0f, 1.0f});

	GUI& gui = this->getGUI();
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
	this->button->addText("Play", "arialBig", glm::vec4(1.0f));
	this->button->setCallback([this](void) {
		this->pushState(new GameState(this->selectedLevel));
	});
	gui.addPanel(this->button);

	this->initPanelLayout();

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

void MenuState::initPanelLayout()
{
	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	unsigned width = 200;
	unsigned height = 300;
	ScrollPanel* scrollPanel = new ScrollPanel(width, height);
	scrollPanel->setOption(GUI::FLOAT_DOWN, 10);
	scrollPanel->setOption(GUI::CENTER_X);
	scrollPanel->setColor(glm::vec4(0.2, 0.2, 0.2, 1.0));

	scrollPanel->addItem([this](void) {
		this->selectedLevel = "./Game/Level/level.json";
	}, "Level 1");

	scrollPanel->addItem([this](void) {
		this->selectedLevel = "./Game/Level/newLevel.json";
	}, "Level 2");

	scrollPanel->addItem([this](void) {
		this->selectedLevel = "./Game/Level/newLevel2.json";
	}, "Level 3");

	this->getGUI().addPanel(scrollPanel);
}

void MenuState::initLevelSelectLayout()
{
}
