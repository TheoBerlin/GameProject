#include "ScrollPanel.h"



ScrollPanel::ScrollPanel(unsigned width, unsigned height, const std::string& font, int scrollSpeed) : Panel()
{
	this->scrollSpeed = scrollSpeed;

	this->setSize(glm::vec2(width, height));

	this->list = new List(font, 8, 0);
	this->list->setSize(glm::vec2((unsigned)(width * 0.8f), (unsigned)(height * 0.9f)));
	this->list->setOption(GUI::CENTER_X);
	this->list->setOption(GUI::FLOAT_UP, (int)(height * 0.05));
	this->list->setColor(glm::vec4(0.9, 0.9, 0.9, 1.0));

	this->addChild(list);

	this->init();
}


ScrollPanel::~ScrollPanel()
{
	EventBus::get().unsubscribe(this, &ScrollPanel::MouseScrollCallback);
}

void ScrollPanel::init()
{
	EventBus::get().subscribe(this, &ScrollPanel::MouseScrollCallback);
}

void ScrollPanel::addItem(std::function<void(void)> func, const std::string& text)
{
	this->list->addItem(text, func);
}

void ScrollPanel::setMaxVisibleItems(unsigned maxItemsVisible)
{
	this->list->setMaxVisibleItems(maxItemsVisible);
}

void ScrollPanel::setItemSpacing(unsigned itemSpacing)
{
	this->list->setItemSpacing(itemSpacing);
}

void ScrollPanel::MouseScrollCallback(MouseScrollEvent * evnt)
{
	this->list->scroll(evnt->yoffset * this->scrollSpeed);
}
