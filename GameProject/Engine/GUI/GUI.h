#pragma once

#include <vector>

class Panel;
class GUI
{
public:
	enum OPTION
	{
		// --------- Size options ---------
		SCALE_TO_TEXT_X = 0,
		SCALE_TO_TEXT_Y,

		// ------- Position options -------
		FLOAT_LEFT,
		FLOAT_DOWN,
		FLOAT_UP,
		FLOAT_RIGHT,
		CENTER_X,
		CENTER_Y,

		FIT_X,
		FIT_Y,

		// --------- Text options ---------
		TEXT_CENTER_X,
		TEXT_CENTER_Y,
		TEXT_FLOAT_LEFT,
		TEXT_FLOAT_RIGHT,
		TEXT_FLOAT_UP,
		TEXT_FLOAT_DOWN,

		OPTIONS_MAX
	};

	struct OPTION_VALUE
	{
		bool b = false;
		float f = 0.0f;
		int i = 0;
		unsigned int ui = 0u;
	};

	GUI();
	virtual ~GUI();

	void addPanel(Panel* panel);
	Panel* getPanel(unsigned int index);

	void update();

	std::vector<Panel*>& getPanelList();

private:
	std::vector<Panel*> panelList;
};
