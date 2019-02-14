#include "Panel.h"

#include "FontManager.h"
#include "Utils/Logger.h"

Panel::Panel() : backgroundTexture(nullptr), bakedTexture(nullptr)
{
	init();
}

Panel::~Panel()
{
	for (auto& text : this->textList)
		delete text.first;
	this->textList.clear();
}

void Panel::setBakedTexture(Texture * texture)
{
	this->bakedTexture = texture;
}

Texture * Panel::getBakedTexture()
{
	return this->bakedTexture;
}

void Panel::setBackgroundTexture(Texture * texture)
{
	this->backgroundTexture = texture;
}

Texture * Panel::getBackgroundTexture()
{
	return this->backgroundTexture;
}

void Panel::setColor(glm::vec4 color)
{
	this->color = color;
}

glm::vec4 Panel::getColor() const
{
	return this->color;
}

void Panel::setPoistion(glm::vec2 pos)
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
}

glm::vec2 Panel::getSize() const
{
	return this->size;
}

void Panel::addText(const std::string & str, float x, float y, float scale, const std::string& font, const glm::vec4& color)
{
	Text* text = new Text();
	text->setColor(color);
	text->updateText(str, scale, FontManager::getFont(font));
	glm::vec2 relativePos = glm::vec2(x, y);
	this->textList.push_back(std::pair<Text*, glm::vec2>(text, relativePos));
}

void Panel::updateText(unsigned int index, const std::string & str, float x, float y, float scale)
{
	if (index >= 0 && index < this->textList.size())
	{
		this->textList[index].first->updateText(str, scale);
		this->textList[index].second = glm::vec2(x, y);
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
		this->textList[index].first->update();
	}
	else
	{
		LOG_WARNING("Index out of bounds");
		return;
	}
}

std::vector<std::pair<Text*, glm::vec2>>& Panel::getTextList()
{
	return textList;
}

void Panel::init()
{
	this->pos = { 0.0f, 0.0f };
	this->size = { 1.0f, 1.0f };
	this->color = { 1.0f, 1.0f, 1.0f, 1.0f };
}
