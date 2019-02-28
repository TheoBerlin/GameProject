#include "List.h"



List::List(const std::string & font, unsigned maxItemsVisible, unsigned itemSpacing)
{
	this->maxItemsVisible = maxItemsVisible;
	this->font = font;
	this->scrollOffset = 0;
	this->itemSpacing = itemSpacing;
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

	glm::vec4 buttonColor(0.6, 0.6, 0.6, 1.0);
	button->setColor(buttonColor);
	button->setHoverColor(buttonColor * 1.2f);
	button->setNormalColor(buttonColor);
	button->setPressedColor(buttonColor * 0.8f);

	button->setOption(GUI::TEXT_CENTER_X);
	button->setOption(GUI::TEXT_CENTER_Y);

	button->addText(text, this->font, glm::vec4(1.0f));

	button->setCallback(func);

	this->listButtons.push_back(button);
	this->addChild(button);
}

void List::scroll(int scrollOffset)
{
	this->scrollOffset += scrollOffset;
	if (this->scrollOffset < 0)
		this->scrollOffset = 0;

	for (size_t i = 0; i < this->listButtons.size(); i++) {
		Button* button = this->listButtons[i];
		int offset = i * (unsigned)(button->getSize().y + this->itemSpacing);
		button->setOption(GUI::FLOAT_UP, offset + this->scrollOffset);
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
