#include "FontManager.h"

#include "Utils/Logger.h"
std::unordered_map<std::string, Font*> FontManager::fonts;

void FontManager::addFont(const std::string & name, const std::string & path, unsigned int size)
{
	std::unordered_map<std::string, Font*>::iterator it = fonts.begin();
	if (fonts.find(name) == fonts.end())
	{
		LOG_SUCCESS("Added font '%s' [%d] with path: %s", name.c_str(), size, path.c_str());
		fonts[name] = new Font(path, size);
	}
}

Font * FontManager::getFont(const std::string & name)
{
	if (fonts.find(name) == fonts.end())
		return nullptr;

	return fonts[name];
}

void FontManager::free()
{
	std::unordered_map<std::string, Font*>::iterator it = fonts.begin();
	for (; it != fonts.end(); it++)
		delete it->second;
	fonts.clear();
}
