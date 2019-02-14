#include "Settings.h"

bool Settings::readFile(std::string fileName)
{
	std::ifstream iFile;
	iFile.open(fileName);
	if (iFile.is_open()) {
		try {
			iFile >> jsonFile;
		}
		catch (const std::exception e) {
			LOG_ERROR("%s: Failed to read JSON file with error: %s", CLASS_NAME, e.what());
			return false;
		}
	}
	else {
		LOG_ERROR("Can not open file: %s", fileName.c_str());
		return false;
	}

	readVolume();
	readScreenWidth();
	readScreenHeight();

	if (iFile.is_open()) {
		iFile.close();
	}

	return true;
}

void Settings::writeFile(std::string fileName)
{
	jsonFile["Volume"] = volume;
	jsonFile["ScreenWidth"] = screenWidth;
	jsonFile["ScreenHeight"] = screenHeight;
	std::ofstream oStream(fileName);
	oStream << std::setw(4) << jsonFile << std::endl;
}

void Settings::readVolume()
{

	json::json& jsonVolume = jsonFile["Volume"];
	if (!jsonVolume.empty()) {
		volume = jsonVolume;
	}
	else {
		LOG_ERROR("%s: Volume has no value", CLASS_NAME);
	}
}

void Settings::readScreenWidth()
{
	json::json& jsonWidth = jsonFile["ScreenWidth"];
	if (!jsonWidth.empty()) {
		screenWidth = jsonWidth;
	}
	else {
		LOG_ERROR("%s: ScreenWidth has no value", CLASS_NAME);
	}
}

void Settings::readScreenHeight()
{
	json::json& jsonHeight = jsonFile["ScreenHeight"];
	if (!jsonHeight.empty()) {
		screenHeight = jsonHeight;
	}
	else {
		LOG_ERROR("%s: Volume has no value", CLASS_NAME);
	}
}

Settings& Settings::get()
{
	static Settings instance;
	return instance;
}

Settings::Settings()
{
	EventBus::get().subscribe(this, &Settings::handleResizeEvent);
	readFile();
}

Settings::~Settings()
{
	if (changed == true) {
		writeFile();
	}
	changed = false;
}

float Settings::getVolume()
{
	return this->volume;
}

void Settings::setVolume(float volume)
{
	this->volume = volume;
	changed = true;
}

int Settings::getScreenWidth()
{
	return this->screenWidth;
}

int Settings::getScreenHeight()
{
	return this->screenHeight;
}

void Settings::handleResizeEvent(WindowResizeEvent * evnt)
{
	setResolution(evnt->width, evnt->height);
}


void Settings::setResolution(int width, int height)
{
	this->screenWidth = width;
	this->screenHeight = height;
	changed = true;
}
