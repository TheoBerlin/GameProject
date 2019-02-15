#pragma once

#include "../Rendering/GLAbstraction/Shader.h"
#include "../Rendering/GLAbstraction/Texture.h"
#include "../Rendering/Display.h"

class Font
{
public:
	struct Character
	{
		GLuint textureID;
		int width;			// Glyph size in X.
		int height;			// Glyph size in Y.
		int bearingX;		// Distance from origin to left side of glyph.
		int bearingY;		// Distance from origin to top side of glyph.
		GLuint advance;		// How far to advance when drawing a new character.
	};

	Font();
	Font(const std::string& pathName, unsigned int size);
	virtual ~Font();

	bool load(const std::string& pathName, unsigned int size);

	void setSize(unsigned int size);

	Shader* getShader();
	Character& getCharacter(char c);
	std::unordered_map<char, Character>& getCharacters();

private:
	void freeCharacters();
	void loadCharacters();
	void updateCharacters();

	std::string pathName;
	static std::unique_ptr<Shader> shader;
	FT_Face face;

	std::unordered_map<char, Character> characters;

};