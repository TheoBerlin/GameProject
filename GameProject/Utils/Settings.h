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

	float volume();
};

inline bool Settings::readFile(std::string fileName)
{
	return false;
}

inline bool Settings::writeFile(std::string fileName)
{
	return false;
}

inline Settings& Settings::get()
{
	static Settings instance;
	return instance;
}

inline Settings::Settings()
{
	readFile();
}

inline Settings::~Settings()
{
	//Add flag if changes are made, to make sure that the file only changes if necessary
	writeFile();
}