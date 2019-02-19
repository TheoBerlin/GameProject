#include "Panel.h"

#include "FontManager.h"
#include "Utils/Logger.h"

#include "../Rendering/GUIRenderer.h"

#include "../Events/EventBus.h"

Panel::Panel() : backgroundTexture(nullptr)
{
	init();
}

Panel::~Panel()
{
	// Delete all text in panel
	for (auto& text : this->textList)
		delete text.first;
	this->textList.clear();

	// Delete children.
	for (Panel* child : this->children)
		delete child;
	this->children.clear();
}

void Panel::setBakedTexture(const Texture& texture)
{
	this->bakedTexture = texture;
}

Texture * Panel::getBakedTexture()
{
	return &this->bakedTexture;
}

void Panel::setBackgroundTexture(Texture * texture)
{
	this->backgroundTexture = texture;
	this->shouldUpdate = true;
}

Texture * Panel::getBackgroundTexture()
{
	return this->backgroundTexture;
}

void Panel::setColor(glm::vec4 color)
{
	this->color = color;
	this->shouldUpdate = true;
}

glm::vec4 Panel::getColor() const
{
	return this->color;
}

void Panel::setPosition(glm::vec2 pos)
{
	this->pos = pos;
}

glm::vec2 Panel::getPosition() const
{
	return this->pos;
}

void Panel::setSize(glm::vec2 size)
{
	this->size = size;
	this->shouldUpdate = true;
}

glm::vec2 Panel::getSize() const
{
	return this->size;
}

void Panel::addText(const std::string & str, const std::string & font, const glm::vec4 & color)
{
	addText(str, 0.0f, 0.0f, font, color);
}

void Panel::addText(const std::string & str, float x, float y, const std::string& font, const glm::vec4& color)
{
	// Construct the text.
	Text* text = new Text();
	text->setColor(color);
	text->updateText(str, FontManager::getFont(font));

	// Set its position and add it to the textList.
	glm::vec2 relativePos = glm::vec2(x, y);
	this->textList.push_back(std::pair<Text*, glm::vec2>(text, relativePos));

	this->shouldUpdate = true;
}

void Panel::updateText(unsigned int index, const std::string & str, float x, float y)
{
	if (index >= 0 && index < this->textList.size()) {
		this->textList[index].first->updateText(str);
		this->textList[index].second = glm::vec2(x, y);
		this->shouldUpdate = true;
	}
	else
	{
		LOG_WARNING("Index out of bounds");
		return;
	}
}

void Panel::updateText(unsigned int index, float x, float y)
{
	if (index >= 0 && index < this->textList.size()) {
		this->textList[index].second = glm::vec2(x, y);
		this->shouldUpdate = true;
	}
	else
	{
		LOG_WARNING("Index out of bounds");
		return;
	}
}

void Panel::updateText(unsigned int index, const std::string & str)
{
	if (index >= 0 && index < this->textList.size()) {
		this->textList[index].first->updateText(str);
		this->shouldUpdate = true;
	}
	else
	{
		LOG_WARNING("Index out of bounds");
		return;
	}
}

void Panel::setTextColor(unsigned int index, const glm::vec4 & color)
{
	if (index >= 0 && index < this->textList.size())
	{
		this->textList[index].first->setColor(color);
		this->shouldUpdate = true;
	}
	else
	{
		LOG_WARNING("Index out of bounds");
		return;
	}
}

void Panel::addChild(Panel * panel)
{
	panel->setParent(this);
	this->children.push_back(panel);
}

void Panel::rebake()
{
	if (this->shouldUpdate)
	{
		processOptions();

		Display& display = Display::get();
		GUIRenderer& guiRenderer = display.getGUIRenderer();
		guiRenderer.prepareTextRendering();
		guiRenderer.bakePanel(this);
		this->shouldUpdate = false;
	}
}

std::vector<std::pair<Text*, glm::vec2>>& Panel::getTextList()
{
	return textList;
}

std::vector<Panel*>& Panel::getChildren()
{
	return this->children;
}

Panel * Panel::getParent()
{
	return this->parent;
}

void Panel::setParent(Panel * parent)
{
	this->parent = parent;
}

void Panel::setOption(GUI::OPTION option)
{
	GUI::OPTION_VALUE v;
	v.f = 0.0f;
	this->options[option] = std::pair<bool, GUI::OPTION_VALUE>(true, v);
	this->shouldUpdate = true;
}

bool Panel::hasUpdated() const
{
	if (this->shouldUpdate) return true;

	// Check if the text has been updated.
	for (auto& t : this->textList)
		if (t.first->hasUpdated())
			return true;

	// Check if the children have been updated.
	for (Panel* p : this->children)
		if(p->hasUpdated())
			return true;

	return false;
}

void Panel::init()
{
	this->pos = { 0.0f, 0.0f };
	this->size = { 1.0f, 1.0f };
	this->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->shouldUpdate = false;
	this->parent = nullptr;

	this->options.resize(GUI::OPTION::OPTIONS_MAX);

	EventBus::get().subscribe(this, &Panel::resizeCallback);
}

void Panel::processOptions()
{
	for (unsigned int i = 0; i < GUI::OPTION::OPTIONS_MAX; i++)
	{
		auto& option = this->options[i];
		if (option.first)
			processOption(option.second, i);
	}
}

void Panel::processOption(GUI::OPTION_VALUE option, unsigned int index)
{
	switch (index)
	{
	// ---------------- Size related -------------------
	case GUI::OPTION::SCALE_TO_TEXT_X:
		processScaleToTextOption(GUI::OPTION::SCALE_TO_TEXT_X, option.f);
		break;
	case GUI::OPTION::SCALE_TO_TEXT_Y:
		processScaleToTextOption(GUI::OPTION::SCALE_TO_TEXT_Y, option.f);
		break;

	// -------------- Position related -----------------
	case GUI::OPTION::FLOAT_LEFT:
		processPositionOption(GUI::OPTION::FLOAT_LEFT, option.f);
		break;
	case GUI::OPTION::FLOAT_RIGHT:
		processPositionOption(GUI::OPTION::FLOAT_RIGHT, option.f);
		break;
	case GUI::OPTION::FLOAT_UP:
		processPositionOption(GUI::OPTION::FLOAT_UP, option.f);
		break;
	case GUI::OPTION::FLOAT_DOWN:
		processPositionOption(GUI::OPTION::FLOAT_DOWN, option.f);
		break;
	case GUI::OPTION::CENTER_X:
		processPositionOption(GUI::OPTION::CENTER_X, 0.0f);
		break;
	case GUI::OPTION::CENTER_Y:
		processPositionOption(GUI::OPTION::CENTER_Y, 0.0f);
		break;

	case GUI::OPTION::FIT_X:
		processFitOption(GUI::OPTION::FIT_X, 0.0f);
		break;
	case GUI::OPTION::FIT_Y:
		processFitOption(GUI::OPTION::FIT_Y, 0.0f);
		break;

	// ---------------- Text related --------------------
	case GUI::OPTION::TEXT_CENTER_X:
		processTextPositionOption(GUI::OPTION::TEXT_CENTER_X, 0.0f);
		break;
	case GUI::OPTION::TEXT_CENTER_Y:
		processTextPositionOption(GUI::OPTION::TEXT_CENTER_Y, 0.0f);
		break;
	case GUI::OPTION::TEXT_FLOAT_LEFT:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_LEFT, option.f);
		break;
	case GUI::OPTION::TEXT_FLOAT_RIGHT:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_RIGHT, option.f);
		break;
	case GUI::OPTION::TEXT_FLOAT_UP:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_UP, option.f);
		break;
	case GUI::OPTION::TEXT_FLOAT_DOWN:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_DOWN, option.f);
		break;

	}
}

void Panel::processPositionOption(unsigned int index, float v)
{
	if (index == GUI::FLOAT_LEFT)
		this->pos.x = v;
	if (index == GUI::FLOAT_DOWN)
		this->pos.y = v;

	Display& display = Display::get();
	float w = (float)display.getWidth();
	float h = (float)display.getHeight();
	if (this->parent != nullptr)
	{
		w = this->parent->getSize().x;
		h = this->parent->getSize().y;
	}

	if (index == GUI::FLOAT_RIGHT)
		this->pos.x = w - this->size.x - v;
	if (index == GUI::FLOAT_UP)
		this->pos.y = h - this->size.y - v;
	if (index == GUI::CENTER_X)
		this->pos.x = std::floor(w * 0.5f - this->size.x*0.5f);
	if (index == GUI::CENTER_Y)
		this->pos.y = std::floor(h * 0.5f - this->size.y*0.5f);
}

void Panel::processTextPositionOption(unsigned int index, float v)
{
	bool centerX = false;
	bool centerY = false;
	if (index == GUI::TEXT_CENTER_X) centerX = true;
	if (index == GUI::TEXT_CENTER_Y) centerY = true;

	char horizontal = 0;
	if (index == GUI::TEXT_FLOAT_LEFT)	horizontal = 1;
	if (index == GUI::TEXT_FLOAT_RIGHT) horizontal = 2;

	char vertical = 0;
	if (index == GUI::TEXT_FLOAT_DOWN)	vertical = 1;
	if (index == GUI::TEXT_FLOAT_UP) vertical = 2;

	for (auto& t : this->textList)
	{
		Text* text = t.first;
		if (centerX)
			t.second.x = std::floor(this->getSize().x*0.5f - text->getWidth()*0.5f);
		if (centerY)
			t.second.y = std::floor(this->getSize().y*0.5f - text->getHeight()*0.5f);

		if (horizontal == 1)
			t.second.x = v;
		if(horizontal == 2)
			t.second.x = this->getSize().x - text->getWidth() - v;

		if (vertical == 1)
			t.second.y = v;
		if (vertical == 2)
			t.second.y = this->getSize().y - text->getHeight() - v;
	}
}

void Panel::processFitOption(unsigned int index, float v)
{
	Display& display = Display::get();
	float w = (float)display.getWidth();
	float h = (float)display.getHeight();
	if (this->parent != nullptr)
	{
		w = this->parent->getSize().x;
		h = this->parent->getSize().y;
	}

	if (index == GUI::FIT_X)
	{
		this->size.x = w;
		this->pos.x = 0.0f;
	}

	if (index == GUI::FIT_Y)
	{
		this->size.y = h;
		this->pos.y = 0.0f;
	}
}

void Panel::processScaleToTextOption(unsigned int index, float v)
{
	if (index == GUI::SCALE_TO_TEXT_X)
	{
		float maxX = 0.0f;
		for (auto& t : this->textList)
		{
			float w = t.first->getWidth();
			if (w > maxX) maxX = w;
		}
		this->size.x = maxX + v;
		this->pos.x = 0.0f;
	}

	if (index == GUI::SCALE_TO_TEXT_Y)
	{
		float maxY = 0.0f;
		for (auto& t : this->textList)
		{
			float h = t.first->getHeight();
			if (h > maxY) maxY = h;
		}
		this->size.y = maxY + v;
		this->pos.y = 0.0f;
	}
}

void Panel::resizeCallback(WindowResizeEvent * evnt)
{
	this->shouldUpdate = true;
	rebake();
}
