#pragma once

#include "Font.h"

class Text
{
public:
	struct CharacterRect
	{
		GLuint textureID;
		struct Rect
		{
			glm::vec4 tl;
			glm::vec4 tr;
			glm::vec4 bl;
			glm::vec4 br;
		} rect;
	};

	Text();
	Text(const std::string& str, Font* font);
	virtual ~Text();

	void setColor(const glm::vec4& color);
	void setText(const std::string& str, Font* font = nullptr);

	glm::vec4 getColor() const;
	float getWidth() const;
	float getHeight() const;
	float getBearingY() const;

	std::vector<CharacterRect>& getCharacterRects();

private:
	std::string str;

	float width;
	float height;
	float bearingY;
	glm::vec4 color;

	Font* font;
	std::vector<CharacterRect> characterRects;
};
