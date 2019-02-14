#include "GUIManager.h"

#include "Utils/Logger.h"

GUIManager::GUIManager()
{
}

GUIManager::~GUIManager()
{
	for (Panel* panel : this->panelList)
		delete panel;
	this->panelList.clear();
}

void GUIManager::addPanel(Panel * panel)
{
	this->panelList.push_back(panel);
}

Panel * GUIManager::getPanel(unsigned int index)
{
	if (index < this->panelList.size())
		return this->panelList[index];
	LOG_WARNING("Index out of bounds!");
	return nullptr;
}

void GUIManager::update()
{
}

std::vector<Panel*>& GUIManager::getPanelList()
{
	return this->panelList;
}
