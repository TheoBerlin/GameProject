#include "GUI.h"

#include "Utils/Logger.h"
#include "Engine/Rendering/Display.h"
#include "Engine/Rendering/GUIRenderer.h"
#include "Panel.h"

GUI::GUI()
{
}

GUI::~GUI()
{
	for (Panel* panel : this->panelList)
		delete panel;
	this->panelList.clear();
}

void GUI::addPanel(Panel * panel)
{
	this->panelList.push_back(panel);
}

bool GUI::removePanel(unsigned int index)
{
	const size_t size = this->panelList.size();
	if (size == 0)
	{
		LOG_WARNING("Trying to remove a panel from an empty list.");
		return false;
	}
	if (index < size)
	{
		delete this->panelList[index];
		this->panelList[index] = this->panelList[size - 1];
		this->panelList.pop_back();
		return true;
	}
	LOG_WARNING("Index out of bounds!");
	return false;
}

bool GUI::removePanel(Panel * panel)
{
	const size_t size = this->panelList.size();
	if (size == 0)
	{
		LOG_WARNING("Trying to remove a panel from an empty list.");
		return false;
	}

	Panel* last = this->panelList[size - 1];
	for (Panel* p : this->panelList)
	{
		if (p == panel)
		{
			delete p;
			p = last;
			this->panelList[size - 1] = nullptr;
			this->panelList.pop_back();
			return true;
		}
	}
	LOG_WARNING("Panel was not found.");
	return false;
}

Panel * GUI::getPanel(unsigned int index)
{
	if (index < this->panelList.size())
		return this->panelList[index];
	LOG_WARNING("Index out of bounds!");
	return nullptr;
}

std::vector<Panel*>& GUI::getPanelList()
{
	return this->panelList;
}