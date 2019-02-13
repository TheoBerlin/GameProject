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
	bool changed = false;
	float volume;
	int screenWidth;
	int screenHeight;

	bool readFile(std::string fileName = "./Utils/Settings.json");
	void writeFile(std::string fileName = "./Utils/Settings.json");

	void readVolume();
	void readScreenWidth();
	void readScreenHeight();

	json::json jsonFile;
public:
	static Settings& get();
	Settings();
	~Settings();

	float getVolume();
	void setVolume(float volume = 0.5f);
	int getScreenWidth();
	void setScreenWidth(int screenWidth = 1280);
	int getScreenHeight();
	void setScreenHeight(int screenHeight = 720);
};


