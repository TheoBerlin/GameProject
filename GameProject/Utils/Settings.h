#pragma once
#include "../Utils/Logger.h"

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

#define CLASS_NAME "SETTINGS"

class Settings 
{
private:
	bool readFile(std::string fileName = "./Utils/Settings.json");
	bool writeFile(std::string fileName = "./Utils/Settings.json");

	json::json jsonFile;
public:
	static Settings& get();
	Settings();
	~Settings();

	float volume();
};

inline bool Settings::readFile(std::string fileName)
{
	std::ifstream iFile;
	iFile.open(fileName);
	if (iFile.is_open())
	{
		try {
			iFile >> jsonFile;
		}
		catch (const std::exception e) {
			LOG_ERROR("%s: Failed to read JSON file with error: %s", CLASS_NAME, e.what());
			return false;
		}
	}
	else
	{
		LOG_ERROR("Can not open file: %s", fileName.c_str());
		return false;
	}
	return true;
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

inline float Settings::volume()
{
	return 0.5f;
}
