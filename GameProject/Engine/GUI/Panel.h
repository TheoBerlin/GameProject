#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include "../Rendering/GLAbstraction/Texture.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "Text.h"
#include "GUI.h"

class Panel
{
public:
	Panel();
	virtual ~Panel();

	void setBakedTexture(const Texture& texture);
	Texture* getBakedTexture();

	void setBackgroundTexture(Texture* texture);
	Texture* getBackgroundTexture();

	void setColor(glm::vec4 color);
	glm::vec4 getColor() const;

	void setPosition(glm::vec2 pos);
	glm::vec2 getPosition() const;

	void setSize(glm::vec2 size);
	glm::vec2 getSize() const;

	void addText(const std::string& str, const std::string& font, const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 1.0f });
	void addText(const std::string& str, float x, float y, const std::string& font, const glm::vec4& color = { 0.0f, 0.0f, 0.0f, 1.0f });
	void updateText(unsigned int index, const std::string& str, float x, float y);
	void updateText(unsigned int index, float x, float y);
	void updateText(unsigned int index, const std::string& str);
	void setTextColor(unsigned int index, const glm::vec4& color);

	void addChild(Panel* panel);

	void rebake();

	std::vector<std::pair<Text*, glm::vec2>>& getTextList();
	std::vector<Panel*>& getChildren();

	Panel* getParent();
	void setParent(Panel* parent);

	template<typename T>
	void setOption(GUI::OPTION option, T value);
	void setOption(GUI::OPTION option);
	void update();
	
	bool hasUpdated() const;

protected:
	void init();
	void processOption(std::pair<bool, GUI::OPTION_VALUE> option, unsigned int index);
	void processPositionOption(unsigned int index, float v);
	void processTextPositionOption(unsigned int index, float v);
	void processFitOption(unsigned int index, float v);
	void processScaleToTextOption(unsigned int index, float v);

	bool shouldUpdate;

	Panel* parent;
	std::vector<Panel*> children;
	std::vector<std::pair<Text*, glm::vec2>> textList;

	Texture bakedTexture;
	Texture* backgroundTexture;

	glm::vec4 color;

	glm::vec2 pos;
	glm::vec2 size;

	std::vector<std::pair<bool, GUI::OPTION_VALUE>> options;
};

template<typename T>
inline void Panel::setOption(GUI::OPTION option, T value)
{
	GUI::OPTION_VALUE v;
	if (typeid(T) == typeid(bool))
		v.b = (bool)value;
	if (typeid(T) == typeid(float))
		v.f = (float)value;
	if (typeid(T) == typeid(int))
		v.i = (int)value;
	if (typeid(T) == typeid(unsigned int))
		v.ui = (unsigned int)value;
	this->options[option] = std::pair<bool, GUI::OPTION_VALUE>(true, v);
	this->shouldUpdate = true;
}
