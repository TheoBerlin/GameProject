#include "SliderPanel.h"

#include "Button.h"
#include "Engine/Rendering/Display.h"
#include "Engine/GUI/GUIColors.h"

SliderPanel::SliderPanel() : Panel()
{
	this->slider = nullptr;
	this->text = nullptr;
	this->background = nullptr;
	this->btn = nullptr;

	this->offset = 0;
	this->size = { 280, 30 };
	this->sliderSize = { 200, 30 };

	this->backgroundColor = PANEL_BACKGROUND_COLOR;
	this->sliderColor = BUTTON_PRESS_COLOR;

	createBackground();
	createButton();
	createSlider();
	createText();
}

SliderPanel::~SliderPanel()
{
}

void SliderPanel::setSliderColor(const glm::vec3& color)
{
	this->sliderColor = color;
	this->slider->setColor({ this->sliderColor, 1.f });
}

void SliderPanel::setBackgroundColor(const glm::vec3& color)
{
	this->backgroundColor = color;
	this->btn->setNormalColor({ this->backgroundColor, 1.f });
	this->btn->setHoverColor({ this->backgroundColor, 1.f });
	this->btn->setPressedColor({ this->backgroundColor, 1.f });
}

void SliderPanel::setSliderCallback(const std::function<void(float)>& callback)
{
	this->callback = callback;
}

float SliderPanel::getSliderFactor()
{
	return (float)this->offset / (float)this->sliderSize.x;
}

void SliderPanel::setSliderFactor(float factor)
{
	this->offset = (int)(factor * this->sliderSize.x);
	this->slider->setOption(GUI::FLOAT_LEFT, this->offset - this->sliderSize.x);
	this->updateText();
}

void SliderPanel::mouseCallback()
{
	double xpos, ypos;
	glfwGetCursorPos(Display::get().getWindowPtr(), &xpos, &ypos);

	this->offset = (int)xpos - this->globalPos.x;
	this->slider->setOption(GUI::FLOAT_LEFT, this->offset - this->sliderSize.x);

	this->updateText();
	callback(this->getSliderFactor());
}

void SliderPanel::createBackground()
{
	this->background = new Panel();
	this->background->setSize(this->sliderSize);
	this->background->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->setColor({ 1.f, 1.f, 1.f, 0.f });
	this->addChild(this->background);
}

void SliderPanel::createSlider()
{
	this->slider = new Panel();
	this->slider->setColor({ this->sliderColor, 1.f});
	this->slider->setOption(GUI::FLOAT_LEFT, this->offset - this->sliderSize.x);
	this->slider->setSize(this->sliderSize);
	this->background->addChild(this->slider);
}

void SliderPanel::createButton()
{
	this->btn = new Button();
	this->btn->setNormalColor({this->backgroundColor, 1.f});
	this->btn->setHoverColor({ this->backgroundColor, 1.f });
	this->btn->setPressedColor({ this->backgroundColor, 1.f });
	this->btn->setOption(GUI::FIT_X);
	this->btn->setOption(GUI::FIT_Y);

	this->btn->setCallback(std::bind(&SliderPanel::mouseCallback, this));
	this->background->addChild(this->btn);
}

void SliderPanel::createText()
{
	this->text = new Panel();
	this->text->setOption(GUI::FLOAT_RIGHT);
	this->text->setOption(GUI::TEXT_CENTER_X);
	this->text->setOption(GUI::TEXT_CENTER_Y);
	this->text->setOption(GUI::SCALE_TO_TEXT_X);
	this->text->setOption(GUI::SCALE_TO_TEXT_Y);
	this->text->setColor({ 1.f, 1.f, 1.f, 0.f });
	std::string str = std::to_string((int)(getSliderFactor() * 100));
	this->text->addText(str + "%", "aldo", TEXT_COLOR);
	this->addChild(this->text);
}

void SliderPanel::updateText()
{
	this->text->updateText(0, std::to_string((int)(getSliderFactor() * 100)) + "%");
}
