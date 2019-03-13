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
	float volume;
	int screenWidth;
	int screenHeight;
	float mouseSensitivity;
	float shadowReScale;

	bool readFile(std::string fileName = "./Utils/Settings.json");
	void writeFile(std::string fileName = "./Utils/Settings.json");

	void readVolume();
	void readScreenWidth();
	void readScreenHeight();
	void readMouseSensitivity();
	void readShadowReScale();

	json::json jsonFile;

public:
	static Settings& get();
	Settings();
	~Settings();

	float getVolume();
	void setVolume(float volume = 0.5f);
	int getScreenWidth();
	int getScreenHeight();
	void setResolution(int width, int height);
	float getMouseSensitivity();
	void setMouseSensitivity(const float mouseSensitivity);
	float getShadowReScale();
	void setShadowReScale(float shadowReScale);
	void handleResizeEvent(WindowResizeEvent * evnt);
};


