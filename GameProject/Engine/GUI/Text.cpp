#include "Text.h"

#include "Utils/Logger.h"
#include "../Rendering/GUIRenderer.h"

Text::Text() : color(0.0f, 0.0f, 0.0f, 1.0f), shouldUpdate(false)
{
	this->font = nullptr;
	this->width = 0;
	this->height = 0;
	this->line = 0;
	this->bearingY = 0;
	this->str = "";
}

Text::Text(const std::string & str, Font * font) : color(0.0f, 0.0f, 0.0f, 1.0f), shouldUpdate(false)
{
	this->line = 0;
	setText(str, font);
}

Text::~Text()
{
}

void Text::setColor(const glm::vec4 & color)
{
	this->color = color;
	this->shouldUpdate = true;
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
		this->shouldUpdate = true;

		// Clear character set.
		this->charactersDrawData.clear();
		this->str = str;

		this->width = 0;
		this->height = 0;

		unsigned int min = 0;
		this->bearingY = 0;

		// Loop through all characters and calculate their position and scale.
		int x = 0;
		const char* c;
		for (c = str.c_str(); *c; c++)
		{
			Font::Character character = this->font->getCharacter(*c);

			int x2 = x + (int)character.bearingX;
			int y2 = -(int)character.height + (int)character.bearingY;
			unsigned int w = character.width;
			unsigned int h = character.height;
			this->width = w;

			// Save the highest and the lowest distances to the line.
			if (min < (character.height - character.bearingY))
				min = character.height - character.bearingY;
			if (this->bearingY < character.bearingY)
				this->bearingY = character.bearingY;

			CharacterDrawData characterData;
			characterData.pos = { x2, y2 };
			characterData.size = { w, h };
			characterData.textureID = character.textureID;
			this->charactersDrawData.push_back(characterData);

			if(*(c+1))
				x += (int)(character.advance >> 6);
		}
		this->width += (unsigned int)x;
		this->height = min + this->bearingY;
		return true;
	}
	return false;
}

void Text::updateText(const std::string & str, Font * font)
{
	setText(str, font);
}

void Text::rebake()
{
	if (this->shouldUpdate)
	{
		Display& display = Display::get();
		GUIRenderer& guiRenderer = display.getGUIRenderer();

		guiRenderer.prepareTextRendering();
		guiRenderer.bakeText(*this);
		this->shouldUpdate = false;
	}
}

glm::vec4 Text::getColor() const
{
	return this->color;
}

unsigned int Text::getWidth() const
{
	return this->width;
}

unsigned int Text::getHeight() const
{
	return this->height;
}

unsigned int Text::getBearingY() const
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

bool Text::hasUpdated() const
{
	return this->shouldUpdate;
}
