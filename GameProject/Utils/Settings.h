#pragma once
#include "./Engine/Events/EventBus.h"

#include "glm/glm.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>

namespace json = nlohmann;

class Settings 
{
private:
	bool changed = false;
	int screenWidth;
	int screenHeight;
	float mouseSensitivity;

	bool readFile(std::string fileName = "./Utils/Settings.json");

	void readScreenWidth();
	void readScreenHeight();
	void readMouseSensitivity();

	json::json jsonFile;

public:
	static Settings& get();
	Settings();
	~Settings();

	void writeFile(std::string fileName = "./Utils/Settings.json");

	void readVolumes();
	int getScreenWidth();
	int getScreenHeight();
	void setResolution(int width, int height);
	float getMouseSensitivity();
	void setMouseSensitivity(const float mouseSensitivity);
	void handleResizeEvent(WindowResizeEvent * evnt);
};


