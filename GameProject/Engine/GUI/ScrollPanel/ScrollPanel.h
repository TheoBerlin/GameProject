#pragma once

#include "Engine/GUI/Panel.h"
#include "Engine/GUI/ScrollPanel/List.h"

#include "Engine/Events/EventBus.h"

#include <vector>

class Button;

class ScrollPanel : public Panel
{
public:
	ScrollPanel(unsigned width = 200, unsigned height = 400, const std::string& font = "aldo", int scrollSpeed = 15);
	~ScrollPanel();

	void init();

	void addItem(std::function<void(void)> func, const std::string& text = "item");

	void setMaxVisibleItems(unsigned maxItemsVisible);
	void setItemSpacing(unsigned itemSpacing);

	// Set active button
	void setActiveButton(unsigned index);

private:
	void MouseScrollCallback(MouseScrollEvent* evnt);
	void MouseClickCallback(MouseClickEvent* evnt);
	
	int scrollSpeed;

	List* list;
	Button* activeButton;
};

