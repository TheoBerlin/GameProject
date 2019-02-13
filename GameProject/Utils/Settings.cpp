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

bool Settings::writeFile(std::string fileName)
{
	return false;
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
	readFile();
}

Settings::~Settings()
{
	//Add flag if changes are made, to make sure that the file only changes if necessary
	writeFile();
}

float Settings::getVolume()
{
	return this->volume;
}

int Settings::getScreenWidth()
{
	return this->screenWidth;
}

int Settings::getScreenHeight()
{
	return this->screenHeight;
}
