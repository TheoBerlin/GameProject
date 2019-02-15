#include "Text.h"

#include "Utils/Logger.h"
#include "../Rendering/GUIRenderer.h"

Text::Text() : color(0.0f, 0.0f, 0.0f, 1.0f), scale(2.0f)
{
	this->font = nullptr;
}

Text::Text(const std::string & str, Font * font) : color(0.0f, 0.0f, 0.0f, 1.0f), scale(2.0f)
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

bool Text::setText(const std::string & str, Font * font)
{
	bool isFontDifferent = false;
	if (font != nullptr)
	{
		if (this->font != font)
		{
			isFontDifferent = true;
			this->font = font;
		}
	}
	else if (this->font == nullptr)
	{
		LOG_WARNING("Text '%s' is missing font!", str.c_str());
		return false;
	}

	// Only update text if the string or the font is different.
	if (this->str != str || isFontDifferent)
	{
		// Clear character set.
		this->charactersDrawData.clear();
		this->str = str;

		this->width = 0.0f;
		this->height = 0.0f;

		float min = 0.0f;
		this->bearingY = 0.0f;

		// Loop through all characters and calculate their position and scale.
		float x = 0.0f;
		const char* c;
		for (c = str.c_str(); *c; c++)
		{
			Font::Character character = this->font->getCharacter(*c);

			float x2 = x + (float)character.bearingX;
			float y2 = -(float)character.height + (float)character.bearingY;
			float w = (float)character.width;
			float h = (float)character.height;
			this->width = w;

			// Save the highest and the lowest distances to the line.
			if (min < (character.height - character.bearingY))
				min = (float)character.height - (float)character.bearingY;
			if (this->bearingY < character.bearingY)
				this->bearingY = (float)character.bearingY;

			CharacterDrawData characterData;
			characterData.pos = { x2, y2 };
			characterData.scale = { w, h };
			characterData.textureID = character.textureID;
			this->charactersDrawData.push_back(characterData);

			x += (character.advance >> 6);
		}
		this->width += x;
		this->height = min + this->bearingY;
		return true;
	}
	return false;
}

void Text::updateText(const std::string & str, float scale, Font * font)
{
	if (setText(str, font))
	{
		Display& display = Display::get();
		GUIRenderer& guiRenderer = display.getGUIRenderer();

		guiRenderer.prepareTextRendering();
		guiRenderer.bakeText(*this, scale);
	}
}

void Text::update()
{
	Display& display = Display::get();
	GUIRenderer& guiRenderer = display.getGUIRenderer();

	guiRenderer.prepareTextRendering();
	guiRenderer.bakeText(*this, this->scale);
}

void Text::setScale(float scale)
{
	this->scale = scale;
}

float Text::getScale() const
{
	return this->scale;
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

void Text::setBakedTexture(const Texture& texture)
{
	this->bakedTexture = texture;
}

Texture* Text::getBakedTexture()
{
	return &this->bakedTexture;
}

Font * Text::getFont()
{
	return this->font;
}

void Text::setFont(Font * font)
{
	this->font = font;
}

std::vector<Text::CharacterDrawData>& Text::getCharactersDrawData()
{
	return this->charactersDrawData;
}
