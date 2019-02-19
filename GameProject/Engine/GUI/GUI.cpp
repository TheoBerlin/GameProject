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