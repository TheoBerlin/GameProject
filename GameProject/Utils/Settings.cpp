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
	readShadowResolutionFactor();
	readFullscreen();

	if (iFile.is_open()) {
		iFile.close();
	}

	return true;
}

void Settings::writeFile(std::string fileName)
{
	jsonFile["MasterVolume"] = SoundManager::get().getMasterVolume();
	jsonFile["MusicVolume"] = SoundManager::get().getMusicVolume();
	jsonFile["EffectVolume"] = SoundManager::get().getEffectsVolume();
	jsonFile["MiscVolume"] = SoundManager::get().getMiscVolume();
	jsonFile["ScreenWidth"] = this->screenWidth;
	jsonFile["ScreenHeight"] = this->screenHeight;
	jsonFile["MouseSensitivity"] = this->mouseSensitivity;
	jsonFile["Fullscreen"] = this->fullscreen;
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
		SoundManager::get().setEffectsVolume(jsonFile["EffectVolume"]);
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

void Settings::readShadowResolutionFactor()
{
	json::json& jsonResolutionFactor = jsonFile["ShadowResolutionFactor"];
	if (!jsonResolutionFactor.empty()) {
		this->shadowResolutionFactor = jsonResolutionFactor;
	}
	else {
		LOG_ERROR("%s: ShadowResolutionFactor has no value");
	}
}

void Settings::readFullscreen()
{
	json::json& jsonFullscreen = jsonFile["Fullscreen"];
	if (!jsonFullscreen.empty()) {
		this->fullscreen = jsonFullscreen;
	}
	else {
		LOG_ERROR("Fullscreen has no value");
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

bool Settings::getFullscreen()
{
	return this->fullscreen;
}

void Settings::setFullscreen(bool fullscreen)
{
	this->fullscreen = fullscreen;
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

float Settings::getShadowResolutionFactor()
{
	return this->shadowResolutionFactor;
}

void Settings::handleResizeEvent(WindowResizeEvent * evnt)
{
	setResolution(evnt->width, evnt->height);
}
