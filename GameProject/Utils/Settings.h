#pragma once
#include "../Utils/Logger.h"

class Settings 
{
private:
	bool readFile(std::string fileName = "Settings.json");
	bool writeFile(std::string fileName = "Settings.json");
public:
	static Settings& get();
	Settings();
	~Settings();

};

inline Settings& Settings::get()
{
	static Settings instance;
	return instance;
}

inline Settings::Settings()
{
	readFile("test");
}

inline Settings::~Settings()
{
}
