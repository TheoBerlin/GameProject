#pragma once

#include "Font.h"
#include <unordered_map>

class FontManager
{
public:
	/*
	Add a font to the map.
	Arguments:
		name: The internal name of the font. This is used when you want to get the font later.
		path: The path to the font file.
		size: The size of the font.
	*/
	static void addFont(const std::string& name, const std::string& path, unsigned int size);

	/*
	Get a pointer of a font.
	Arguments:
		name: The internal name of the font. This was set when you added the font.
	*/
	static Font* getFont(const std::string& name);

	/*
	Free all the memory of the fonts.
	*/
	static void free();
private:
	static std::unordered_map<std::string, Font*> fonts;
};