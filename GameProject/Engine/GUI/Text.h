#pragma once

#include "Font.h"
#include "../Rendering/GLAbstraction/Texture.h"

class Text
{
public:
	struct CharacterDrawData
	{
		GLuint textureID;
		glm::vec2 pos;
		glm::vec2 scale;
	};

	Text();
	Text(const std::string& str, Font* font);
	virtual ~Text();

	/*
	Set the color of the text.
	*/
	void setColor(const glm::vec4& color);

	/*
	Upadate the string and construct the new set of characters. Will only update the character set if the string or the font is different.
	Arguments:
		str: The string which will be converted to characters.
		font: A pointer to the font, which will be used. If font is nullptr, an error will occur.
	Return:
		true if updated, false if not.
	*/
	bool setText(const std::string& str, Font* font = nullptr);

	/*
	Update the text like setText but will only do this if the string or the font is different.
	Arguments:
		str: The string which will be converted to characters.
		scale: The scale of the text. This is not the same as the font size.
		font: A pointer to the font, which will be used. If font is nullptr, an error will occur.
	*/
	void updateText(const std::string& str, float scale = 2.0f, Font* font = nullptr);

	/*
	Update the data and bake it into a texture.
	*/
	void update();

	/*
	Set the scale of the text. This is not the same as setting the font size.
	*/
	void setScale(float scale);

	/*
	Get the current scale.
	*/
	float getScale() const;

	/*
	Get the color as rgba.
	*/
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

	/*
	Set the baked texture as a copy of the argument.
	*/
	void setBakedTexture(const Texture& texture);
	/*
	Get a pointer to the baked texture.
	*/
	Texture* getBakedTexture();

	/*
	Get a pointer to the current font.
	*/
	Font* getFont();

	/*
	Set the current font.
	*/
	void setFont(Font* font);

	/*
	Get the characters draw data set.
	*/
	std::vector<CharacterDrawData>& getCharactersDrawData();

private:
	std::string str;

	float line;
	float width;
	float height;
	float bearingY;
	glm::vec4 color;

	float scale;

	Texture bakedTexture;
	Font* font;
	std::vector<CharacterDrawData> charactersDrawData;
};
