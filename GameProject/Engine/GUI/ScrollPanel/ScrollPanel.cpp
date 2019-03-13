#include "ScrollPanel.h"

#include "Engine/GUI/Button.h"
#include "Engine/GUI/GUIColors.h"

ScrollPanel::ScrollPanel(unsigned width, unsigned height, const std::string& font, int scrollSpeed) : Panel()
{
	this->scrollSpeed = scrollSpeed;

	this->setSize(glm::vec2(width, height));

	this->list = new List(font, 8, 0);
	this->list->setSize(glm::vec2((unsigned)(width * 0.8f), (unsigned)(height * 0.9f)));
	this->list->setOption(GUI::CENTER_X);
	this->list->setOption(GUI::FLOAT_UP, (int)(height * 0.05));
	this->list->setColor(PANEL_BACKGROUND_COLOR);

	this->addChild(list);

	this->init();
}


ScrollPanel::~ScrollPanel()
{
	EventBus::get().unsubscribe(this, &ScrollPanel::MouseScrollCallback);
	EventBus::get().unsubscribe(this, &ScrollPanel::MouseClickCallback);

}

void ScrollPanel::init()
{
	EventBus::get().subscribe(this, &ScrollPanel::MouseScrollCallback);
	EventBus::get().subscribe(this, &ScrollPanel::MouseClickCallback);
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

void ScrollPanel::setActiveButton(unsigned index)
{
	if (index < this->list->getListButtons().size())
	{
		this->activeButtonIndex = index;
		this->activeButton = this->list->getListButtons()[index];
		this->activeButton->setNormalColor({ 0.4f, 0.f, 0.6f, 1.f });
	}
}

unsigned ScrollPanel::getActiveButtonIndex() const
{
	return this->activeButtonIndex;
}

void ScrollPanel::MouseScrollCallback(MouseScrollEvent * evnt)
{
	this->list->scroll(evnt->yoffset * this->scrollSpeed);
}

void ScrollPanel::MouseClickCallback(MouseClickEvent * evnt)
{
	if (evnt->button == GLFW_MOUSE_BUTTON_1 && evnt->action == GLFW_RELEASE)
	{
		double xpos, ypos;
		int height = Display::get().getHeight();
		glfwGetCursorPos(Display::get().getWindowPtr(), &xpos, &ypos);
		glm::ivec2 gPos = this->list->getGlobalPosition();
		// If inside the list
		if (xpos > gPos.x && xpos < gPos.x + this->list->getSize().x
			&& height - ypos > gPos.y && height - ypos < gPos.y + this->list->getSize().y)
		{
			// Check which button it is
			for (unsigned i = 0; i < this->list->getListButtons().size(); i++)
			{
				Button* current = this->list->getListButtons()[i];
				if (current->hover() && current != this->activeButton)
				{
					current->setNormalColor({ 0.4f, 0.f, 0.6f, 1.f });
					if (this->activeButton)
					{
						this->activeButton->setNormalColor(this->list->getBackgroundColor());
					}
					this->activeButtonIndex = i;
					this->activeButton = current;
					break;
				}
			}
		}
	}
}
