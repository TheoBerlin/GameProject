#include "MenuState.h"

#include "../../Engine/States/StateManager.h"
#include "GameState.h"
#include "TestState.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/GUIRenderer.h"
#include "../../Engine/GUI/FontManager.h"
#include "glm/vec4.hpp"
#include "../../Engine/InputHandler.h"
#include "Utils/Logger.h"
#include "../../Engine/GUI/Button.h"

MenuState::MenuState() : State()
{
	FontManager::addFont("times", "./Game/assets/fonts/times/times.ttf", 16);
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arialbd.ttf", 16);
	FontManager::addFont("arialBig", "./Game/assets/fonts/arial/arialbd.ttf", 36);
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
	this->button->setHoverColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	this->button->setNormalColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	this->button->setPressedColor({ 0.0f, 0.0f, 1.0f, 1.0f });
	this->button->addText("Play", "arialBig");
	this->button->setCallback([](void) { LOG_INFO("Pressed"); });
	gui.addPanel(this->button);

	InputHandler ih(Display::get().getWindowPtr());
}

MenuState::~MenuState()
{
}

void MenuState::start()
{
	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	this->panel = new Panel();
	this->panel->setSize({ 100, 100 });
	this->panel->setOption(GUI::FLOAT_UP);
	this->panel->setOption(GUI::FLOAT_RIGHT);
	this->panel->setOption(GUI::TEXT_CENTER_X);
	this->panel->setOption(GUI::TEXT_FLOAT_DOWN);
	this->panel->setColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	this->panel->addText("Play", "arial", { 0.0f, 1.0f, 0.0f, 1.0f });
	this->getGUI().addPanel(this->panel);

	Panel* p = new Panel();
	p->setSize({ 50, 50 });
	p->setOption(GUI::FLOAT_LEFT, 5);
	p->setOption(GUI::FLOAT_UP, 10);
	p->setBackgroundTexture(TextureManager::loadTexture("./Game/assets/heaven.png", TextureType::TXTYPE_DIFFUSE));
	p->addText("Inner", "arial");
	this->panel->addChild(p);
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
		this->panel->updateText(0, "FPS: " + std::to_string((int)(1.0f / dt)));
		time = 0.0f;
	}

	if (this->button->isActivated())
	{
		this->pushState(new GameState());
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
