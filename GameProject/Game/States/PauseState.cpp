#include "PauseState.h"

#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/GUIRenderer.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Events/EventBus.h"

#include "../../Engine/GUI/Text.h"
#include "../../Engine/GUI/Panel.h"
#include "../../Engine/GUI/Button.h"

PauseState::PauseState()
{
	//Blur scene and copy over into texture, which can be used to draw
	Renderer& renderer = Display::get().getRenderer();
	Texture* latestFrame = renderer.getPipeline()->getFbo()->getColorTexture(0);
	this->backgroundTexture = *renderer.drawTextureToFbo(latestFrame, SHADERS_POST_PROCESS::BLUR_FILTER);

	this->initPanelLayout();

	this->hasSubscribed = false;
}

PauseState::~PauseState()
{
}

void PauseState::start()
{
	// Unlock cursor
	glfwSetInputMode(Display::get().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);


}

void PauseState::end()
{
	Display& display = Display::get();

	// Hide cursor
	glfwSetCursorPos(display.getWindowPtr(), display.getWidth() / 2.0f, display.getHeight() / 2.0f);
	glfwSetInputMode(display.getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Pause game event
	EventBus::get().unsubscribe(this, &PauseState::unpauseGame);
}

void PauseState::update(const float dt)
{
	if (!this->hasSubscribed) {
		//Pause game event
		EventBus::get().subscribe(this, &PauseState::unpauseGame);
		this->hasSubscribed = true;
	}
}

void PauseState::updateLogic(const float dt)
{
}

void PauseState::render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	Display& display = Display::get();

	Renderer& renderer = display.getRenderer();

	renderer.drawTextureToScreen(&backgroundTexture, SHADERS_POST_PROCESS::NO_FILTER);

	GUIRenderer& guiRenderer = display.getGUIRenderer();

	guiRenderer.draw(this->getGUI());
}

void PauseState::initPanelLayout()
{

	GUI& gui = this->getGUI();

	Button* button = new Button();
	button->setOption(GUI::SCALE_TO_TEXT_X, 5);
	button->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	button->setOption(GUI::CENTER_X);
	button->setOption(GUI::CENTER_Y);
	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);
	button->setHoverColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	button->setNormalColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	button->setPressedColor({ 0.0f, 0.0f, 1.0f, 1.0f });
	button->addText("Return", "arialBig");
	button->setCallback([this](void) {
		//this->getGUI().removePanel(this->button);
		this->popState();

	});
	gui.addPanel(button);

	button = new Button();
	button->setOption(GUI::SCALE_TO_TEXT_X, 5);
	button->setOption(GUI::SCALE_TO_TEXT_Y, 5);
	button->setOption(GUI::FLOAT_RIGHT);
	button->setOption(GUI::CENTER_Y);
	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);
	button->setHoverColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	button->setNormalColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	button->setPressedColor({ 0.0f, 0.0f, 1.0f, 1.0f });
	button->addText("Exit", "arialBig");
	button->setCallback([this](void) {
		//this->getGUI().removePanel(this->button);
		this->popState(2);
	});

	gui.addPanel(button);
}

void PauseState::unpauseGame(KeyEvent * ev)
{
	if (ev->key == GLFW_KEY_ESCAPE && ev->action == GLFW_PRESS) {
		EventBus::get().unsubscribe(this, &PauseState::unpauseGame);
		this->popState();
	}
}
