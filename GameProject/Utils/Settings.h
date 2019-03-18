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
	int screenWidth = 0;
	int screenHeight = 0;
	float mouseSensitivity = 0.5f;
	float shadowResolutionFactor = 1.f;

	bool readFile(std::string fileName = "./Utils/Settings.json");

	void readScreenWidth();
	void readScreenHeight();
	void readMouseSensitivity();
	void readShadowResolutionFactor();

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
	float getShadowResolutionFactor();
	void handleResizeEvent(WindowResizeEvent * evnt);
};


