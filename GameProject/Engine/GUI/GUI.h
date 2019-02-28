#pragma once

#include <vector>

class Panel;
class GUI
{
public:
	enum OPTION
	{
		// ---------------------------------- Size options ----------------------------------

		/*
		Used when want to match the size of the panel to the text which it holds.
		Value:
			Padding in x in pixels.
		*/
		SCALE_TO_TEXT_X = 0,

		/*
		Used when want to match the size of the panel to the text which it holds.
		Value:
			Padding in y in pixels.
		*/
		SCALE_TO_TEXT_Y,

		// ---------------------------------- Position options ----------------------------------

		/*
		Will make the panel move to the left to match its parent's left side's position or the display's left side's position.
		Value:
			Margin to the left in pixels.
		*/
		FLOAT_LEFT,

		/*
		Will make the panel move to the bottom to match its parent's bottom side's position or the display's bottom side's position.
		Value:
			Margin down in pixels.
		*/
		FLOAT_DOWN,

		/*
		Will make the panel move to the top to match its parent's top side's position or the display's top side's position.
		Value:
			Margin to the top in pixels.
		*/
		FLOAT_UP,

		/*
		Will make the panel move to the right to match its parent's right side's position or the display's right side's position.
		Value:
			Margin to the right in pixels.
		*/
		FLOAT_RIGHT,

		/*
		This will center the panels position, in the x-axis, inside its parent or the display.
		*/
		CENTER_X,

		/*
		This will center the panels position, in the y-axis, inside its parent or the display.
		*/
		CENTER_Y,

		/*
		This will match the panels size in x to its parent or the display.
		*/
		FIT_X,

		/*
		This will match the panels size in y to its parent or the display.
		*/
		FIT_Y,

		// ---------------------------------- Text options ----------------------------------

		/*
		This will center the text in the x-axis inside the panel.
		*/
		TEXT_CENTER_X,

		/*
		This will center the text in the y-axis inside the panel.
		*/
		TEXT_CENTER_Y,

		/*
		This will move the text to the left to match the panel's left side's position.
		Value:
			Margin to the left in pixels.
		*/
		TEXT_FLOAT_LEFT,

		/*
		This will move the text to the right to match the panel's right side's position.
		Value:
			Margin to the right in pixels.
		*/
		TEXT_FLOAT_RIGHT,

		/*
		This will move the text to the top to match the panel's top side's position.
		Value:
			Margin to the top in pixels.
		*/
		TEXT_FLOAT_UP,

		/*
		This will move the text to the bottom to match the panel's bottom side's position.
		Value:
			Margin to the bottom in pixels.
		*/
		TEXT_FLOAT_DOWN,

		OPTIONS_MAX
	};

	// A strcut to hold the value of the option. 
	struct OPTION_VALUE
	{
		int i = 0;
	};

	GUI();
	virtual ~GUI();

	/*
	Add a panel to the gui.
	Arguments:
		panel: A pointer to the panel which will be added.
	*/
	void addPanel(Panel* panel);

	/*
	Remove a panel from the gui. This will swap the panel at the position of index with the last panel and delete it.
	Arguments:
		index: The index of the panel in the list of gui elements.
	return:
		true if removed else false.
	*/
	bool removePanelI(unsigned int index);

	/*
	Remove a panel from the gui. This will swap the panel with the last panel and delete it.
	Arguments:
		panel: A pointer to the panel which will be removed.
	return:
		true if removed else false.
	*/
	bool removePanel(Panel* panel);

	/*
	Get a pointer to a panel from the gui.
	Arguments:
		index: The index of the panel in the list of gui elements.
	*/
	Panel* getPanel(unsigned int index);

	/*
	Get the list of panels in the gui.
	*/
	std::vector<Panel*>& getPanelList();

	/*
	Remove all panels from the list.
	*/
	void clearAllPanels();

	void setActive(bool active);

private:
	std::vector<Panel*> panelList;
};