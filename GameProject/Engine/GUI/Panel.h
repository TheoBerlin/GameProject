#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include "../Rendering/GLAbstraction/Texture.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "Text.h"

class Panel
{
public:
	Panel();
	virtual ~Panel();

	void setBakedTexture(Texture* texture);
	Texture* getBakedTexture();

	void setBackgroundTexture(Texture* texture);
	Texture* getBackgroundTexture();

	void setColor(glm::vec4 color);
	glm::vec4 getColor() const;

	void setPosition(glm::vec2 pos);
	glm::vec2 getPosition() const;

	void setSize(glm::vec2 size);
	glm::vec2 getSize() const;

	void addText(const std::string& str, float x, float y, float scale, const std::string& font, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	void updateText(unsigned int index, const std::string& str, float x, float y, float scale = -1.0f);
	void updateText(unsigned int index, const std::string& str, float scale = 0.0f);
	void setTextColor(unsigned int index, const glm::vec4& color);

	std::vector<std::pair<Text*, glm::vec2>>& getTextList();

private:
	void init();

	std::vector<Panel*> panels;
	std::vector<std::pair<Text*, glm::vec2>> textList;

	Texture* bakedTexture;
	Texture* backgroundTexture;

	glm::vec4 color;

	glm::vec2 pos;
	glm::vec2 size;
};
