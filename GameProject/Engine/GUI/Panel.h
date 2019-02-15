#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include "../Rendering/GLAbstraction/Texture.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "Text.h"

#include "../Events/Events.h"

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

	void addText(const std::string& str, float x, float y, float scale, const std::string& font, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	void updateText(unsigned int index, const std::string& str, float x, float y, float scale = -1.0f);
	void updateText(unsigned int index, const std::string& str, float scale = 0.0f);
	void setTextColor(unsigned int index, const glm::vec4& color);

	void addChild(Panel* panel);

	void rebake();

	std::vector<std::pair<Text*, glm::vec2>>& getTextList();
	std::vector<Panel*>& getChildren();

private:
	void init();
	void rebakeCallback(WindowResizeEvent* evnt);

	std::vector<Panel*> children;
	std::vector<std::pair<Text*, glm::vec2>> textList;

	Texture bakedTexture;
	Texture* backgroundTexture;

	glm::vec4 color;

	glm::vec2 pos;
	glm::vec2 size;
};
