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

	glm::vec4 textColor = { 0.9f, 0.9f, 0.9f, 1.0f };
	glm::vec4 backgroundColor = { 0.1f, 0.1f, 0.1f, 0.99f };
	glm::vec4 hoverColor = { 0.5f, 0.0f, 0.5f, 1.0f };
	glm::vec4 pressColor = { 0.3f, 0.0f, 0.3f, 1.0f };

	Button* button = new Button();
	button->setOption(GUI::SCALE_TO_TEXT_X, 25);
	button->setOption(GUI::SCALE_TO_TEXT_Y, 25);
	button->setOption(GUI::CENTER_X);
	button->setOption(GUI::CENTER_Y);
	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);
	button->setHoverColor(hoverColor);
	button->setNormalColor(backgroundColor);
	button->setPressedColor(pressColor);
	button->addText("RETURN", "aldo", textColor);
	button->setCallback([this](void) {
		this->popState();

	});
	gui.addPanel(button);

	button = new Button();
	button->setOption(GUI::SCALE_TO_TEXT_X, 25);
	button->setOption(GUI::SCALE_TO_TEXT_Y, 25);
	button->setOption(GUI::CENTER_X);
	button->setOption(GUI::CENTER_Y, -(int)(button->getSize().y + 25));
	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);
	button->setHoverColor(hoverColor);
	button->setNormalColor(backgroundColor);
	button->setPressedColor(pressColor);
	button->addText("EXIT", "aldo", textColor);
	button->setCallback([this](void) {
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
