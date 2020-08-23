#pragma once

#include "../Rendering/GLAbstraction/Shader.h"
#include "../Rendering/GLAbstraction/Texture.h"
#include "../Rendering/Display.h"
#include <memory>

class Font
{
public:
	struct Character
	{
		GLuint textureID;
		unsigned int width;		// Glyph size in X.
		unsigned int height;	// Glyph size in Y.
		unsigned int bearingX;	// Distance from origin to left side of glyph.
		unsigned int bearingY;	// Distance from origin to top side of glyph.
		GLuint advance;			// How far to advance when drawing a new character.
	};

	Font();
	/*
	Construct a font from file.
	Arguments:
		pathName: Path to font file.
		size: The size of the font.
	*/
	Font(const std::string& pathName, unsigned int size);
	virtual ~Font();

	/*
	Construct a font from file.
	Arguments:
		pathName: Path to font file.
		size: The size of the font.
	*/
	bool load(const std::string& pathName, unsigned int size);

	/*
	Set the size of the font. This will construct a new font.
	*/
	void setSize(unsigned int size);

	/*
	Get a pointer to the font shader.
	*/
	Shader* getShader();

	/*
	Get a single character.
	Arguments:
		c: The character to fetch.
	Returns:
		A referance to the character.
	*/
	Character& getCharacter(char c);

	/*
	Get the map of characters.
	*/
	std::unordered_map<char, Character>& getCharacters();

private:
	/*
	Free the memory of the characters.
	*/
	void freeCharacters();
	
	/*
	Load all 125 characters into the map.
	*/
	void loadCharacters();

	/*
	Clear the map of characters and load them again. 
	*/
	void updateCharacters();

private:
	std::string pathName;
	static std::unique_ptr<Shader> shader;
	FT_Face face;

	std::unordered_map<char, Character> characters;

};