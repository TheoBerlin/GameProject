#include "Text.h"

#include "Utils/Logger.h"

Text::Text() : color(0.0f, 0.0f, 0.0f, 1.0f)
{
	this->font = nullptr;
}

Text::Text(const std::string & str, Font * font) : color(0.0f, 0.0f, 0.0f, 1.0f)
{
	setText(str, font);
}

Text::~Text()
{
}

void Text::setColor(const glm::vec4 & color)
{
	this->color = color;
}

void Text::setText(const std::string & str, Font * font)
{
	if (font != nullptr)
		this->font = font;
	else
	{
		LOG_WARNING("Font is missing!");
		return;
	}

	this->characterRects.clear();
	this->str = str;

	this->width = 0.0f;
	this->height = 0.0f;

	float x = 0.0f;
	const char* c;
	for (c = str.c_str(); *c; c++)
	{
		Font::Character character = this->font->getCharacter(*c);

		float x2 = x + character.bearingX;
		float y2 = character.bearingY;
		float w = character.width;
		float h = character.height;

		this->width += x2 + w;
		if (this->height < h)
			this->height = h;
		if (this->bearingY < y2)
			this->bearingY = y2;

		CharacterRect characterRect;
		characterRect.textureID = character.textureID;
		characterRect.rect.tl = { x2,	 y2,	 0, 0 };
		characterRect.rect.tr = { x2 + w, y2,	 1, 0 };
		characterRect.rect.bl = { x2,	 y2 + h, 0, 1 };
		characterRect.rect.br = { x2 + w, y2 + h, 1, 1 };
		this->characterRects.push_back(characterRect);

		x += (character.advance >> 6);
	}
}

glm::vec4 Text::getColor() const
{
	return this->color;
}

float Text::getWidth() const
{
	return this->width;
}

float Text::getHeight() const
{
	return this->height;
}

float Text::getBearingY() const
{
	return this->bearingY;
}

std::vector<Text::CharacterRect>& Text::getCharacterRects()
{
	return this->characterRects;
}
