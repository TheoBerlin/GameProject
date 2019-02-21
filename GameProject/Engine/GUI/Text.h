#pragma once

#include "Font.h"
#include "../Rendering/GLAbstraction/Texture.h"

class Text
{
public:
	struct CharacterDrawData
	{
		GLuint textureID;
		glm::ivec2 pos;
		glm::ivec2 size;
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
		font: A pointer to the font, which will be used. If font is nullptr, an error will occur.
	*/
	void updateText(const std::string& str, Font* font = nullptr);

	/*
	Bake the data into a texture.
	*/
	void rebake();

	/*
	Get the color as rgba.
	*/
	glm::vec4 getColor() const;

	/*
	Get the width in pixels.
	*/
	unsigned int getWidth() const;
	/*
	Get the height in pixels.
	*/
	unsigned int getHeight() const;
	/*
	Get the bearing in pixels.
	*/
	unsigned int getBearingY() const;

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

	bool hasUpdated() const;

private:
	std::string str;

	bool shouldUpdate;

	unsigned int line;
	unsigned int width;
	unsigned int height;
	unsigned int bearingY;
	glm::vec4 color;

	Texture bakedTexture;
	Font* font;
	std::vector<CharacterDrawData> charactersDrawData;
};
