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
	FontManager::addFont("arial", "./Game/assets/fonts/arial/arial.ttf", 22);
	FontManager::addFont("eu", "./Game/assets/fonts/europeanTypewriter/EuropeanTypewriter.ttf", 22);
	FontManager::addFont("segoeScript", "./Game/assets/fonts/SegoeScript/segoesc.ttf", 22);
	Font* font = FontManager::getFont("eu");
	test.setText("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890 (Testing text rendering!) [%&'-'\"+\"]", font);
	test.setColor({1.0f, 0.0f, 0.0f, 1.0f});
}

MenuState::~MenuState()
{
}

void MenuState::start()
{
	//this->pushState(new GameState());
}

void MenuState::end()
{
}

void MenuState::update(const float dt)
{
}

void MenuState::updateLogic()
{
}

void MenuState::render()
{
	glEnable(GL_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	guiRenderer.prepareTextRendering();
	guiRenderer.bakeText(test, 1.0f);

	guiRenderer.draw(test, -1.0f, -0.5f, 2.0f);
	guiRenderer.draw("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890 (Testing text rendering!) [%&'-'\"+\"]", -1.0f, 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "times", 2.0f);
	
	guiRenderer.draw(test, 0.0f, 0.0f);
}
