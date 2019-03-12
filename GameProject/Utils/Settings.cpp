#include "Settings.h"

#include "Utils/Logger.h"
#include "Engine/Sound/SoundManager.h"

bool Settings::readFile(std::string fileName)
{
	std::ifstream iFile;
	iFile.open(fileName);
	if (iFile.is_open()) {
		try {
			iFile >> jsonFile;
		}
		catch (const std::exception e) {
			LOG_ERROR("Failed to read JSON file with error: %s", e.what());
			return false;
		}
	}
	else {
		LOG_ERROR("Can not open file: %s", fileName.c_str());
		return false;
	}

	readVolumes();
	readScreenWidth();
	readScreenHeight();
	readMouseSensitivity();

	if (iFile.is_open()) {
		iFile.close();
	}

	return true;
}

void Settings::writeFile(std::string fileName)
{
	jsonFile["MasterVolume"] = SoundManager::get().getMasterVolume();
	jsonFile["MusicVolume"] = SoundManager::get().getMusicVolume();
	jsonFile["EffectVolume"] = SoundManager::get().getEffectVolume();
	jsonFile["MiscVolume"] = SoundManager::get().getMiscVolume();
	jsonFile["ScreenWidth"] = screenWidth;
	jsonFile["ScreenHeight"] = screenHeight;
	std::ofstream oStream(fileName);
	oStream << std::setw(4) << jsonFile << std::endl;
}

void Settings::readVolumes()
{
	if (!jsonFile["MasterVolume"].empty()) {
		SoundManager::get().setMasterVolume(jsonFile["MasterVolume"]);
	}
	else {
		LOG_ERROR("Master Volume has no value");
	}
	if (!jsonFile["MusicVolume"].empty()) {
		SoundManager::get().setMusicVolume(jsonFile["MusicVolume"]);
	}
	else {
		LOG_ERROR("Music Volume has no value");
	}
	if (!jsonFile["EffectVolume"].empty()) {
		SoundManager::get().setEffectVolume(jsonFile["EffectVolume"]);
	}
	else {
		LOG_ERROR("Effect Volume has no value");
	}
	if (!jsonFile["MiscVolume"].empty()) {
		SoundManager::get().setMiscVolume(jsonFile["MiscVolume"]);
	}
	else {
		LOG_ERROR("Misc Volume has no value");
	}
}

void Settings::readScreenWidth()
{
	json::json& jsonWidth = jsonFile["ScreenWidth"];
	if (!jsonWidth.empty()) {
		screenWidth = jsonWidth;
	}
	else {
		LOG_ERROR("%s: ScreenWidth has no value");
	}
}

void Settings::readScreenHeight()
{
	json::json& jsonHeight = jsonFile["ScreenHeight"];
	if (!jsonHeight.empty()) {
		screenHeight = jsonHeight;
	}
	else {
		LOG_ERROR("%s: ScreenHeight has no value");
	}
}

void Settings::readMouseSensitivity()
{
	json::json& jsonMouseSens = jsonFile["MouseSensitivity"];
	if (!jsonMouseSens.empty()) {
		mouseSensitivity = jsonMouseSens;
	} else {
		LOG_ERROR("MouseSensitivity has no value");
	}
}

void Settings::readShadowReScale()
{
	json::json& jsonShadowScale = jsonFile["ShadowReScale"];
	if (!jsonShadowScale.empty()) {
		shadowReScale = jsonShadowScale;
	}
	else {
		LOG_ERROR("%s: ShadowReScale has no value");
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
	if (changed == true) {
		writeFile();
	}
	changed = false;
}

int Settings::getScreenWidth()
{
	return this->screenWidth;
}

int Settings::getScreenHeight()
{
	return this->screenHeight;
}

void Settings::setResolution(int width, int height)
{
	this->screenWidth = width;
	this->screenHeight = height;
	changed = true;
}

float Settings::getMouseSensitivity()
{
	return mouseSensitivity;
}

void Settings::setMouseSensitivity(const float mouseSensitivity)
{
	this->mouseSensitivity = mouseSensitivity;

	changed = true;
}

float Settings::getShadowReScale()
{
	return this->shadowReScale;
}

void Settings::setShadowReScale(float shadowReScale)
{
	this->shadowReScale = shadowReScale;
	changed = true;
}

void Settings::handleResizeEvent(WindowResizeEvent * evnt)
{
	setResolution(evnt->width, evnt->height);
}
