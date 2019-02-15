#pragma once

#include "Font.h"
#include <unordered_map>

class FontManager
{
public:
	static void addFont(const std::string& name, const std::string& path, unsigned int size);
	static Font* getFont(const std::string& name);

	static void free();
private:
	static std::unordered_map<std::string, Font*> fonts;
};