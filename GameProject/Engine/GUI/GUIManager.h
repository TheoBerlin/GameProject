#pragma once

#include <vector>
#include "Panel.h"

class GUIManager
{
public:
	GUIManager();
	virtual ~GUIManager();

	void addPanel(Panel* panel);
	Panel* getPanel(unsigned int index);

	void update();

	std::vector<Panel*>& getPanelList();

private:
	std::vector<Panel*> panelList;
};
