#include "MenuState.h"

#include "../../Engine/States/StateManager.h"
#include "GameState.h"
#include "TestState.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/GUIRenderer.h"
#include "../../Engine/GUI/FontManager.h"
#include "glm/vec4.hpp"

MenuState::MenuState() : State()
{
	FontManager::addFont("times", "./Game/assets/fonts/times/times.ttf", 22);
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arialbd.ttf", 18);
	FontManager::addFont("segoeScript", "./Game/assets/fonts/SegoeScript/segoesc.ttf", 22);
	this->font = FontManager::getFont("arial");
	test.setText("------", this->font);
	test.setColor({1.0f, 1.0f, 1.0f, 1.0f});

	GUIManager& guiManager = this->getGUIManager();
	Panel* panel = new Panel();
	panel->setSize({ 0.2f, 0.2f });
	panel->setPosition({ 0.5f, 0.5f });
	panel->addText("Test", 0.0f, 0.0f, 2.0f, "arial");
	guiManager.addPanel(panel);
}

MenuState::~MenuState()
{
}

void MenuState::start()
{
	this->pushState(new GameState());
	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	this->panel = new Panel();
	this->panel->setSize({ 0.2f, 0.2f });
	this->panel->setPosition({-0.5f, -0.5f});
	this->panel->setColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	this->panel->addText("Play", 0.0f, 0.0f, 2.0f, "arial");
	this->getGUIManager().addPanel(this->panel);
}

void MenuState::end()
{
}

void MenuState::update(const float dt)
{
	static float time = 0.0f;
	time += dt;
	if (time > 0.1f)
	{
		this->panel->updateText(0, "FPS: " + std::to_string((int)(1.0f / dt)));
		time = 0.0f;
	}
}

void MenuState::updateLogic()
{
}

void MenuState::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	Display& display = Display::get();
	
	GUIRenderer& guiRenderer = display.getGUIRenderer();
	guiRenderer.draw(this->getGUIManager());
	
	/*
	guiRenderer.prepareTextRendering();
	guiRenderer.draw(this->panel);

	guiRenderer.drawBaked(test, -1.0f, 0.5f);
	*/
	//guiRenderer.draw(test, -1.0, -0.5, 2.0f);
}
