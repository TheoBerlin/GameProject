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
	float volume;
	int screenWidth = 1;
	int screenHeight = 1;

	bool readFile(std::string fileName = "./Utils/Settings.json");
	bool writeFile(std::string fileName = "./Utils/Settings.json");

	void readVolume();
	void readScreenWidth();
	void readScreenHeight();

	json::json jsonFile;
public:
	static Settings& get();
	Settings();
	~Settings();

	float getVolume();
	int getScreenWidth();
	int getScreenHeight();
};


