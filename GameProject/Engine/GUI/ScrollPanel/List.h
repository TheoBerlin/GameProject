#pragma once

#include "Engine/GUI/Button.h"

class List : public Panel
{
public:
	List(const std::string & font, unsigned maxItemsVisible = 10, unsigned itemSpacing = 10);
	~List();

	void addItem(const std::string& text, std::function<void(void)> func);
	void scroll(int scrollOffset);

	void setMaxVisibleItems(unsigned maxItemsVisible);
	void setItemSpacing(unsigned itemSpacing);

private:
	int scrollOffset;

	std::string font;
	unsigned maxItemsVisible;
	unsigned itemSpacing;

	std::vector<Button*> listButtons;
};

