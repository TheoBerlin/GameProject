#include "Panel.h"

#include "FontManager.h"
#include "Utils/Logger.h"

#include "../Rendering/GUIRenderer.h"

#include "../Events/EventBus.h"

Panel::Panel() : backgroundTexture(nullptr), bakedTexture(nullptr)
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

	delete this->bakedTexture;

	EventBus::get().unsubscribe(this, &Panel::resizeCallback);
}

void Panel::setBakedTexture(const Texture& texture)
{
	if (this->bakedTexture == nullptr)
		this->bakedTexture = new Texture(texture);
	else
		*this->bakedTexture = texture;
}

Texture * Panel::getBakedTexture()
{
	return this->bakedTexture;
}

void Panel::setBackgroundTexture(Texture * texture)
{
	this->backgroundTexture = texture;
	this->shouldUpdate = true;
	this->shouldUpdateOptions = true;
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

glm::ivec2 Panel::getGlobalPosition() const
{
	return this->globalPos;
}

void Panel::setPosition(glm::ivec2 pos)
{
	this->pos = pos;
}

glm::ivec2 Panel::getPosition() const
{
	return this->pos;
}

void Panel::setSize(glm::uvec2 size)
{
	this->size = size;
	this->shouldUpdate = true;
	this->shouldUpdateOptions = true;
}

glm::uvec2 Panel::getSize() const
{
	return this->size;
}

void Panel::addText(const std::string & str, const std::string & font, const glm::vec4 & color)
{
	addText(str, 0, 0, font, color);
}

void Panel::addText(const std::string & str, int x, int y, const std::string& font, const glm::vec4& color)
{
	// Construct the text.
	Text* text = new Text();
	text->setColor(color);
	text->updateText(str, FontManager::getFont(font));

	// Set its position and add it to the textList.
	glm::ivec2 relativePos = glm::ivec2(x, y);
	this->textList.push_back(std::pair<Text*, glm::ivec2>(text, relativePos));

	this->shouldUpdate = true;
	this->shouldUpdateOptions = true;
}

void Panel::updateText(unsigned int index, const std::string & str, int x, int y)
{
	if (index >= 0 && index < this->textList.size()) {
		this->textList[index].first->updateText(str);
		this->textList[index].second = glm::ivec2(x, y);
		this->shouldUpdate = true;
		this->shouldUpdateOptions = true;
	}
	else
	{
		LOG_WARNING("Index out of bounds");
		return;
	}
}

void Panel::updateText(unsigned int index, int x, int y)
{
	if (index >= 0 && index < this->textList.size()) {
		this->textList[index].second = glm::ivec2(x, y);
		this->shouldUpdate = true;
		this->shouldUpdateOptions = true;
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
		this->shouldUpdateOptions = true;
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
	if (this->shouldUpdateOptions && this->shown)
	{
		processOptions();
		this->shouldUpdateOptions = false;
	}

	if (hasUpdated())
	{
		if (this->shown)
		{
			updateGlobalPosition();
			Display& display = Display::get();
			GUIRenderer& guiRenderer = display.getGUIRenderer();
			guiRenderer.prepareTextRendering();
			guiRenderer.bakePanel(this);
		}
		this->shouldUpdate = false;
	}
}

std::vector<std::pair<Text*, glm::ivec2>>& Panel::getTextList()
{
	return textList;
}

Text * Panel::getText(const unsigned & index)
{
	if (index > textList.size() - 1)
		return nullptr;
	return textList[index].first;
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

void Panel::removeAllOptions()
{
	for (auto& option : this->options)
		option.first = false;
}

void Panel::removeOption(GUI::OPTION option)
{
	this->options[option].first = false;
}

void Panel::setOption(GUI::OPTION option, int value)
{
	bool update = false;
	GUI::OPTION_VALUE v;
	v.i = value;
	this->options[option] = std::pair<bool, GUI::OPTION_VALUE>(true, v);
	if (shouldUpdate)
		update = true;
	this->shouldUpdate = true;
	this->shouldUpdateOptions = true;
	switch (option)
	{
	case GUI::OPTION::FLOAT_LEFT:
	{
		this->options[GUI::OPTION::FLOAT_RIGHT].first = false;
		if (!this->parent)
			this->shouldUpdate = false;
		break;
	}
	case GUI::OPTION::FLOAT_RIGHT:
	{
		this->options[GUI::OPTION::FLOAT_LEFT].first = false;
		if (!this->parent)
			this->shouldUpdate = false;
		break;
	}
	case GUI::OPTION::FLOAT_UP:
	{
		this->options[GUI::OPTION::FLOAT_DOWN].first = false;
		if (!this->parent)
			this->shouldUpdate = false;
		break;
	}
	case GUI::OPTION::FLOAT_DOWN:
	{
		this->options[GUI::OPTION::FLOAT_UP].first = false;
		if (!this->parent)
			this->shouldUpdate = false;
		break;
	}

	case GUI::OPTION::TEXT_FLOAT_LEFT:
		this->options[GUI::OPTION::TEXT_FLOAT_RIGHT].first = false;
		break;
	case GUI::OPTION::TEXT_FLOAT_RIGHT:
		this->options[GUI::OPTION::TEXT_FLOAT_LEFT].first = false;
		break;
	case GUI::OPTION::TEXT_FLOAT_UP:
		this->options[GUI::OPTION::TEXT_FLOAT_DOWN].first = false;
		break;
	case GUI::OPTION::TEXT_FLOAT_DOWN:
		this->options[GUI::OPTION::TEXT_FLOAT_UP].first = false;
		break;

	case GUI::OPTION::CENTER_X:
	{
		if (!this->parent)
			this->shouldUpdate = false;
		break;
	}
	case GUI::OPTION::CENTER_Y:
	{
		if (!this->parent)
			this->shouldUpdate = false;
		break;
	}
	}

	if (update)
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
		if (p->hasUpdated())
			return true;

	return false;
}

void Panel::setActive(bool active)
{
	this->active = active;
	for (Panel* p : this->children)
		p->setActive(active);
}

bool Panel::isActive() const
{
	return this->active;
}

void Panel::hide()
{
	this->shown = false;
	setActive(false);
	this->shouldUpdate = true;

	unsigned char data[4];
	memset(data, 0, 4);

	if (!this->bakedTexture)
		this->bakedTexture = new Texture(data, 1, 1);
	else
		this->bakedTexture->update(data, 1, 1);
}

void Panel::show()
{
	this->shown = true;
	setActive(true);
	this->shouldUpdate = true;
	this->shouldUpdateOptions = true;
}

bool Panel::isShown() const
{
	return this->shown;
}

void Panel::init()
{
	this->pos = { 0, 0 };
	this->size = { 100, 100 };
	this->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->shouldUpdate = true;
	this->shouldUpdateOptions = false;
	this->parent = nullptr;
	this->active = true;
	this->shown = true;

	this->options.resize(GUI::OPTION::OPTIONS_MAX);

	EventBus::get().subscribe(this, &Panel::resizeCallback);
}

void Panel::processOptions()
{
	for (unsigned int i = 0; i < GUI::OPTION::OPTIONS_MAX; i++)
	{
		auto& option = this->options[i];
		if (option.first)
			processOption(option, i);
	}
}

void Panel::processOption(std::pair<bool, GUI::OPTION_VALUE>& option, unsigned int index)
{
	switch (index)
	{
		// ---------------- Size related -------------------
	case GUI::OPTION::SCALE_TO_TEXT_X:
		processScaleToTextOption(GUI::OPTION::SCALE_TO_TEXT_X, option.second.i);
		break;
	case GUI::OPTION::SCALE_TO_TEXT_Y:
		processScaleToTextOption(GUI::OPTION::SCALE_TO_TEXT_Y, option.second.i);
		break;
	case GUI::OPTION::SCALE_TEXTURE_TO_WIDTH:
		processScaleToTextureOption(GUI::OPTION::SCALE_TEXTURE_TO_WIDTH, option.second.i);
		break;
	case GUI::OPTION::SCALE_TEXTURE_TO_HEIGHT:
		processScaleToTextureOption(GUI::OPTION::SCALE_TEXTURE_TO_HEIGHT, option.second.i);
		break;


		// -------------- Position related -----------------
	case GUI::OPTION::FLOAT_LEFT:
		processPositionOption(GUI::OPTION::FLOAT_LEFT, option.second.i);
		break;
	case GUI::OPTION::FLOAT_RIGHT:
		processPositionOption(GUI::OPTION::FLOAT_RIGHT, option.second.i);
		break;
	case GUI::OPTION::FLOAT_UP:
		processPositionOption(GUI::OPTION::FLOAT_UP, option.second.i);
		break;
	case GUI::OPTION::FLOAT_DOWN:
		processPositionOption(GUI::OPTION::FLOAT_DOWN, option.second.i);
		break;
	case GUI::OPTION::CENTER_X:
		processPositionOption(GUI::OPTION::CENTER_X, option.second.i);
		break;
	case GUI::OPTION::CENTER_Y:
		processPositionOption(GUI::OPTION::CENTER_Y, option.second.i);
		break;

	case GUI::OPTION::FIT_X:
		processFitOption(GUI::OPTION::FIT_X, 0);
		break;
	case GUI::OPTION::FIT_Y:
		processFitOption(GUI::OPTION::FIT_Y, 0);
		break;

		// ---------------- Text related --------------------
	case GUI::OPTION::TEXT_CENTER_X:
		processTextPositionOption(GUI::OPTION::TEXT_CENTER_X, 0);
		break;
	case GUI::OPTION::TEXT_CENTER_Y:
		processTextPositionOption(GUI::OPTION::TEXT_CENTER_Y, 0);
		break;
	case GUI::OPTION::TEXT_FLOAT_LEFT:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_LEFT, option.second.i);
		break;
	case GUI::OPTION::TEXT_FLOAT_RIGHT:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_RIGHT, option.second.i);
		break;
	case GUI::OPTION::TEXT_FLOAT_UP:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_UP, option.second.i);
		break;
	case GUI::OPTION::TEXT_FLOAT_DOWN:
		processTextPositionOption(GUI::OPTION::TEXT_FLOAT_DOWN, option.second.i);
		break;

	}
}

void Panel::processPositionOption(unsigned int index, int v)
{
	if (index == GUI::FLOAT_LEFT)
		this->pos.x = v;
	if (index == GUI::FLOAT_DOWN)
		this->pos.y = v;

	Display& display = Display::get();
	unsigned int w = (unsigned int)display.getWidth();
	unsigned int h = (unsigned int)display.getHeight();
	if (this->parent != nullptr)
	{
		w = this->parent->getSize().x;
		h = this->parent->getSize().y;
	}

	if (index == GUI::FLOAT_RIGHT)
		this->pos.x = (int)w - (int)this->size.x - v;
	if (index == GUI::FLOAT_UP)
		this->pos.y = (int)h - (int)this->size.y - v;
	if (index == GUI::CENTER_X)
		this->pos.x = (int)w / 2 - (int)this->size.x / 2 + v;
	if (index == GUI::CENTER_Y)
		this->pos.y = (int)h / 2 - (int)this->size.y / 2 + v;
}

void Panel::processTextPositionOption(unsigned int index, int v)
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
			t.second.x = this->getSize().x / 2 - text->getWidth() / 2;
		if (centerY)
			t.second.y = this->getSize().y / 2 - text->getHeight() / 2;

		if (horizontal == 1)
			t.second.x = v;
		if (horizontal == 2)
			t.second.x = (int)this->getSize().x - (int)text->getWidth() - v;

		if (vertical == 1)
			t.second.y = (unsigned int)v;
		if (vertical == 2)
			t.second.y = (int)this->getSize().y - (int)text->getHeight() - v;
	}
}

void Panel::processFitOption(unsigned int index, int v)
{
	Display& display = Display::get();
	unsigned int w = (unsigned int)display.getWidth();
	unsigned int h = (unsigned int)display.getHeight();
	if (this->parent != nullptr)
	{
		w = this->parent->getSize().x;
		h = this->parent->getSize().y;
	}

	if (index == GUI::FIT_X)
	{
		this->size.x = w;
		this->pos.x = 0;
	}

	if (index == GUI::FIT_Y)
	{
		this->size.y = h;
		this->pos.y = 0;
	}
}

void Panel::processScaleToTextOption(unsigned int index, int v)
{
	if (index == GUI::SCALE_TO_TEXT_X)
	{
		unsigned int maxX = 0;
		for (auto& t : this->textList)
		{
			unsigned int w = t.first->getWidth();
			if (w > maxX) maxX = w;
		}
		this->size.x = (unsigned int)((int)maxX + v);
		//this->pos.x = 0;
	}

	if (index == GUI::SCALE_TO_TEXT_Y)
	{
		unsigned int maxY = 0;
		for (auto& t : this->textList)
		{
			unsigned int h = t.first->getHeight();
			if (h > maxY) maxY = h;
		}
		this->size.y = (unsigned int)((int)maxY + v);
		//this->pos.y = 0;
	}
}

void Panel::processScaleToTextureOption(unsigned int index, int v)
{
	if (!this->backgroundTexture)
		return;

	if (index == GUI::SCALE_TEXTURE_TO_WIDTH)
	{
		float ratio = (float)this->backgroundTexture->getHeight() / (float)this->backgroundTexture->getWidth();
		if (v == 0)
		{
			this->size.x = this->backgroundTexture->getWidth();
			this->size.y = (unsigned)(size.x * ratio);
		}
		else
		{
			this->size.x = v;
			this->size.y = (unsigned)(v * ratio);
		}
	}

	if (index == GUI::SCALE_TEXTURE_TO_HEIGHT)
	{
		float ratio = (float)this->backgroundTexture->getWidth() / (float)this->backgroundTexture->getHeight();
		if (v == 0)
		{
			this->size.y = this->backgroundTexture->getHeight();
			this->size.x = (unsigned)(size.y * ratio);
		}
		else
		{
			this->size.y = v;
			this->size.x = (unsigned)(v * ratio);
		}
	}
}

void Panel::resizeCallback(WindowResizeEvent * evnt)
{
	this->shouldUpdate = true;
	rebake();
}

void Panel::updateGlobalPosition()
{
	if (this->parent != nullptr)
		this->globalPos = this->parent->getGlobalPosition() + this->pos;
	else
		this->globalPos = this->pos;
}
