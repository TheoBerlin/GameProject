#include "List.h"



List::List(const std::string & font, unsigned maxItemsVisible, unsigned itemSpacing)
{
	this->maxItemsVisible = maxItemsVisible;
	this->font = font;
	this->scrollOffset = 0;
	this->itemSpacing = itemSpacing;

	this->backgroundColor = { 0.1f, 0.1f, 0.1f, 0.99f };
}


List::~List()
{
}

void List::addItem(const std::string& text, std::function<void(void)> func)
{
	Button* button = new Button();

	button->setSize(glm::vec2(this->getSize().x, this->getSize().y / this->maxItemsVisible));

	int offset = this->listButtons.size() * (unsigned)(button->getSize().y + this->itemSpacing);
	button->setOption(GUI::FLOAT_UP, offset);
	button->setOption(GUI::CENTER_X);

	glm::vec4 textColor = { 0.9f, 0.9f, 0.9f, 1.0f };
	glm::vec4 hoverColor = { 0.5f, 0.0f, 0.5f, 1.0f };
	glm::vec4 pressColor = { 0.3f, 0.0f, 0.3f, 1.0f };
	button->setHoverColor(hoverColor);
	button->setNormalColor(this->backgroundColor);
	button->setPressedColor(pressColor);

	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);

	button->addText(text, this->font, textColor);

	button->setCallback(func);

	this->listButtons.push_back(button);
	this->addChild(button);
}

void List::scroll(int scrollOffset)
{
	this->scrollOffset += scrollOffset;
	if (this->listButtons.size() != 0 && this->listButtons.size() > this->maxItemsVisible)
	{
		unsigned btnSize = (unsigned)(this->listButtons[0]->getSize().y + this->itemSpacing);
		int lowLimit = -(int)((this->listButtons.size() * btnSize) - this->maxItemsVisible * btnSize);
		if (this->scrollOffset > 0)
			this->scrollOffset = 0;
		if (this->scrollOffset < lowLimit)
			this->scrollOffset = lowLimit;

		for (size_t i = 0; i < this->listButtons.size(); i++) {
			Button* button = this->listButtons[i];
			int offset = i * (unsigned)(button->getSize().y + this->itemSpacing);
			button->setOption(GUI::FLOAT_UP, offset + this->scrollOffset);
		}
	}
}

void List::setMaxVisibleItems(unsigned maxItemsVisible)
{
	this->maxItemsVisible = maxItemsVisible;
	this->scroll(0);
}

void List::setItemSpacing(unsigned itemSpacing)
{
	this->itemSpacing = itemSpacing;
	this->scroll(0);
}

std::vector<Button*>& List::getListButtons()
{
	return this->listButtons;
}

glm::vec4 & List::getBackgroundColor()
{
	return this->backgroundColor;
}