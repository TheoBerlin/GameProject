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
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arialbd.ttf", 52);
	FontManager::addFont("segoeScript", "./Game/assets/fonts/SegoeScript/segoesc.ttf", 22);
	this->font = FontManager::getFont("arial");
	test.setText("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890 (Testing text rendering!) [%&'-'\"+\"]", this->font);
	test.setColor({1.0f, 1.0f, 1.0f, 1.0f});

	this->panel = new Panel();
	this->panel->setColor({ 0.2f, 0.2f, 0.2f, 1.0f });
}

MenuState::~MenuState()
{
	delete this->panel;
}

void MenuState::start()
{
	//this->pushState(new GameState());
	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	guiRenderer.prepareTextRendering();
	guiRenderer.bakeText(test, 2.0f);
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
		test.updateText("FPS: " + std::to_string((int)(1.0f / dt)), 2.0f);
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

	guiRenderer.draw(this->panel);
	guiRenderer.prepareTextRendering();
	guiRenderer.drawBaked(test, -1.0f, 0.5f);
	//guiRenderer.draw(test, -1.0, -0.5, 2.0f);
}
