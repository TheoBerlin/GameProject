#pragma once

#include "Font.h"
#include "../Rendering/GLAbstraction/Texture.h"

class Text
{
public:
	struct CharacterRect
	{
		GLuint textureID;
		struct Rect
		{
			glm::vec4 tl;
			glm::vec4 bl;
			glm::vec4 tr;
			glm::vec4 br;
		} rect;
	};

	Text();
	Text(const std::string& str, Font* font);
	virtual ~Text();

	void setColor(const glm::vec4& color);
	void setText(const std::string& str, Font* font = nullptr);

	glm::vec4 getColor() const;

	/*
	Get the width in pixels.
	*/
	float getWidth() const;
	/*
	Get the height in pixels.
	*/
	float getHeight() const;
	/*
	Get the bearing in pixels.
	*/
	float getBearingY() const;

	void setTexture(Texture* texture);
	Texture* getTexture();

	Font* getFont();

	std::vector<CharacterRect>& getCharacterRects();

private:
	std::string str;

	float width;
	float height;
	float bearingY;
	glm::vec4 color;

	Texture* texture;
	Font* font;
	std::vector<CharacterRect> characterRects;
};
