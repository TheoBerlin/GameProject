#pragma once

#include "Sound.h"
#include <vector>

class SoundManager {
private:
	std::vector<Sound*> sounds;

	//Volume between 0.0 and 1.0
	float masterVolume = 1.0f;
	float musicVolume = 1.0f;
	float effectVolume = 1.0f;
	float ambientVolume = 1.0f;
	float miscVolume = 1.0f;

	bool errorCheck();

	SoundManager() = default;
	SoundManager(const SoundManager& other) = delete;
public:
	static SoundManager& get();
	virtual ~SoundManager() {};

	void addSound(Sound *sound, SoundType type);
	void removeSound(Sound *sound);

	std::vector<Sound*>* getSoundsVector();

	void setListenerPosition(glm::vec3 position);
	glm::vec3 getListenerPosition() const;
	void setListenerOrientation(glm::vec3 at, glm::vec3 up);
	glm::vec3 getListenerOrientation() const;

	void setMasterVolume(float volume);
	float getMasterVolume() const;
	void setMusicVolume(float volume);
	float getMusicVolume() const;
	void setEffectVolume(float volume);
	float getEffectVolume() const;
	void setAmbientVolume(float volume);
	float getAmbientVolume() const;
	void setMiscVolume(float volume);
	float getMiscVolume() const;
};