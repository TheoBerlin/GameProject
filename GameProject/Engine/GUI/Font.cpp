#include "Font.h"

#include "Utils/Logger.h"

std::unique_ptr<Shader> Font::shader;

Font::Font()
{
	this->face = nullptr;
	if (!shader)
		shader = std::move(std::unique_ptr<Shader>(new Shader("./Engine/Rendering/Shaders/Font.vert", "./Engine/Rendering/Shaders/Font.frag")));
}

Font::Font(const std::string & pathName, unsigned int size)
{
	this->face = nullptr;
	if (!shader)
		shader = std::move(std::unique_ptr<Shader>(new Shader("./Engine/Rendering/Shaders/Font.vert", "./Engine/Rendering/Shaders/Font.frag")));
	load(pathName, size);
}

Font::~Font()
{
	freeCharacters();
	FT_Done_Face(this->face);
}

bool Font::load(const std::string & pathName, unsigned int size)
{
	Display& display = Display::get();
	// Create new face from font.
	if (FT_New_Face(display.getFTLibrary(), pathName.c_str(), 0, &this->face))
	{
		LOG_ERROR("Could not load font: '%s'", pathName.c_str());
		return false;
	}
	FT_Set_Pixel_Sizes(this->face, 0, size);

	this->pathName = pathName;

	freeCharacters();
	loadCharacters();
	return true;
}

void Font::setSize(unsigned int size)
{
	FT_Set_Pixel_Sizes(this->face, 0, size);
	updateCharacters();
}

Shader * Font::getShader()
{
	return shader.get();
}

Font::Character & Font::getCharacter(char c)
{
	return this->characters[c];
}

std::unordered_map<char, Font::Character>& Font::getCharacters()
{
	return this->characters;
}

void Font::freeCharacters()
{
	// Delete textures.
	std::unordered_map<char, Character>::iterator it;
	for (it = this->characters.begin(); it != this->characters.end(); it++)
		glDeleteTextures(1, &it->second.textureID);
}

void Font::loadCharacters()
{
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character.
		if (FT_Load_Char(this->face, c, FT_LOAD_RENDER))
		{
			LOG_WARNING("Could not load character '%c' for font: %s", c, pathName.c_str());
			continue;
		}

		// Set character data.
		Character character;
		character.width = this->face->glyph->bitmap.width;
		character.height = this->face->glyph->bitmap.rows;
		character.bearingX = (unsigned int)this->face->glyph->bitmap_left;
		character.bearingY = (unsigned int)this->face->glyph->bitmap_top;
		character.advance = (unsigned int)this->face->glyph->advance.x;

		// Create texture for glyph.
		GLuint texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, character.width, character.height, 0, GL_RED, GL_UNSIGNED_BYTE, this->face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		character.textureID = texId;

		// Add character to map.
		this->characters[c] = character;
	}
}

void Font::updateCharacters()
{
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character.
		if (FT_Load_Char(this->face, c, FT_LOAD_RENDER))
		{
			LOG_WARNING("Could not load character '%c' for font: %s", c, pathName.c_str());
			continue;
		}

		// Update character data.
		this->characters[c].width = this->face->glyph->bitmap.width;
		this->characters[c].height = this->face->glyph->bitmap.rows;
		this->characters[c].bearingX = (unsigned int)this->face->glyph->bitmap_left;
		this->characters[c].bearingY = (unsigned int)this->face->glyph->bitmap_top;
		this->characters[c].advance = (unsigned int)this->face->glyph->advance.x;

		// Update texture for glyph.
		glBindTexture(GL_TEXTURE_2D, this->characters[c].textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->characters[c].width, this->characters[c].height, 0, GL_RED, GL_UNSIGNED_BYTE, this->face->glyph->bitmap.buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
