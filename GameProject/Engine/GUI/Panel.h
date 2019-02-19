#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include "../Rendering/GLAbstraction/Texture.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "Text.h"
#include "GUI.h"

struct WindowResizeEvent;
class Panel
{
public:
	Panel();
	virtual ~Panel();

	/*
	Set the baked texture. This is the texture which is used when rendering. This will not rebake the texture!
	Arguments:
		texture: The new baked texture. This function will copy the texture.
	*/
	void setBakedTexture(const Texture& texture);
	
	/*
	Get a pointer to the baked texture.
	*/
	Texture* getBakedTexture();

	/*
	Set the background texture. This will not rebake the texture!
	*/
	void setBackgroundTexture(Texture* texture);
	
	/*
	Get a pointer to the background texture.
	*/
	Texture* getBackgroundTexture();

	/*
	Set the color of the panel. This will be multiplied by the texture color when rendered. This will not rebake the texture!
	*/
	void setColor(glm::vec4 color);

	/*
	Get the color of the panel.
	*/
	glm::vec4 getColor() const;

	/*
	Set the poisition in pixels. This will not rebake the texture!
	*/
	void setPosition(glm::uvec2 pos);

	/*
	Get the position in pixels.
	*/
	glm::uvec2 getPosition() const;

	/*
	Set the size in pixels. This will not rebake the texture!
	*/
	void setSize(glm::uvec2 size);

	/*
	Get the size in pixels.
	*/
	glm::uvec2 getSize() const;

	/*
	Add a text to the panel. This will not rebake the texture!
	Arguments:
		str: The string to the converted and rendered.
		font: The font to use when converting the string to a character set.
		color: The color of the text when rendering.
	*/
	void addText(const std::string& str, const std::string& font, const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 1.0f });

	/*
	Add a text to the panel. This will not rebake the texture!
	Arguments:
		str: The string to the converted and rendered.
		x: The x coordinate in pixels. 0 is the left side.
		y: The y coordinate in pixels. 0 is the right side.
		font: The font to use when converting the string to a character set.
		color: The color of the text when rendering.
	*/
	void addText(const std::string& str, unsigned int x, unsigned int y, const std::string& font, const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 1.0f });

	/*
	Update the string and position for an already added text. This will not rebake the texture!
	Arguments:
		index: The index in which the text in was added.
		str: The new string to be converted and renered.
		x: The x coordinate in pixels. 0 is the left side.
		y: The y coordinate in pixels. 0 is the right side.
	*/
	void updateText(unsigned int index, const std::string& str, unsigned int x, unsigned int y);

	/*
	Update the position for an already added text. This will not rebake the texture!
	Arguments:
	index: The index in which the text in was added.
		x: The x coordinate in pixels. 0 is the left side.
		y: The y coordinate in pixels. 0 is the right side.
	*/
	void updateText(unsigned int index, unsigned int x, unsigned int y);

	/*
	Update the string for an already added text. This will not rebake the texture!
	Arguments:
		index: The index in which the text in was added.
		str: The new string to be converted and renered.
	*/
	void updateText(unsigned int index, const std::string& str);

	/*
	Set the color for an already added text.
	Arguments:
		index: The index in which the text in was added.
		color: The new color in rgba [0.0f, 1.0f].
	*/
	void setTextColor(unsigned int index, const glm::vec4& color);

	/*
	Add a child panel. When baked this will be baked separately and then added to this panel's baked texture.
	*/
	void addChild(Panel* panel);

	/*
	This will rebake the panel, its children and text, if a change has occurred.
	*/
	void rebake();

	/*
	Get a list of the current text elements.
	Return:
		A list of pairs. Each pair contain a pointer to the text and its relative position in pixels.
	*/
	std::vector<std::pair<Text*, glm::uvec2>>& getTextList();

	/*
	Get a list of the current children attached to the panel.
	Return:
		A list of pointers to the children.
	*/
	std::vector<Panel*>& getChildren();

	/*
	Get a pointer to the parent panel.
	*/
	Panel* getParent();

	/*
	Set the parent for this panel.
	*/
	void setParent(Panel* parent);

	/*
	Set an option to use. The list of options can be found in the GUI::OPTION enum.
	Arguments:
		option: A enum for how to position or scale the panel and its text.
		value: Depends on the option. It is often an offset in pixels
	*/
	template<typename T>
	void setOption(GUI::OPTION option, T value);

	/*
	Set an option to use. The list of options can be found in the GUI::OPTION enum.
	Arguments:
		option: A enum for how to position or scale the panel and its text.
	*/
	void setOption(GUI::OPTION option);
	
	/*
	Returns true if some data has been changed, otherwise false. 
	*/
	bool hasUpdated() const;

private:

	/*
	Initialize the panel.
	*/
	void init();

	/*
	Process all options.
	*/
	void processOptions();

	/*
	Process a single option.
	Arguments:
		option: The value of the option.
		index: The index of the option. This corresponds directly to the option. A index of 0 is the first enum value.
	*/
	void processOption(GUI::OPTION_VALUE option, unsigned int index);

	/*
	Process the options related to positioning the panel.
	Arguments:
		index: The index of the option. This corresponds directly to the option. A index of 0 is the first enum value.
		v: The value of the option.
	*/
	void processPositionOption(unsigned int index, unsigned int v);

	/*
	Process the options related to positioning the text in the panel.
	Arguments:
		index: The index of the option. This corresponds directly to the option. A index of 0 is the first enum value.
		v: The value of the option.
	*/
	void processTextPositionOption(unsigned int index, unsigned int v);

	/*
	Process the options related to fit the panel in its parent of display if not parent.
	Arguments:
		index: The index of the option. This corresponds directly to the option. A index of 0 is the first enum value.
		v: The value of the option.
	*/
	void processFitOption(unsigned int index, unsigned int v);

	/*
	Process the options related to scaling the panel to match the text.
	Arguments:
		index: The index of the option. This corresponds directly to the option. A index of 0 is the first enum value.
		v: The value of the option.
	*/
	void processScaleToTextOption(unsigned int index, unsigned int v);

	/*
	A callback for when the window is resized. This will updated all the option and rebaked the panel.
	*/
	void resizeCallback(WindowResizeEvent* evnt);

protected:
	bool shouldUpdate;

	Panel* parent;
	std::vector<Panel*> children;
	std::vector<std::pair<Text*, glm::uvec2>> textList;

	Texture bakedTexture;
	Texture* backgroundTexture;

	glm::vec4 color;

	glm::uvec2 pos;
	glm::uvec2 size;

	std::vector<std::pair<bool, GUI::OPTION_VALUE>> options;
};

template<typename T>
inline void Panel::setOption(GUI::OPTION option, T value)
{
	GUI::OPTION_VALUE v;
	if (typeid(T) == typeid(unsigned int))
		v.ui = (unsigned int)value;
	this->options[option] = std::pair<bool, GUI::OPTION_VALUE>(true, v);
	this->shouldUpdate = true;
}
