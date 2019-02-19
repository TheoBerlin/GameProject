#pragma once

#include "Sound.h"
#include <vector>

static class SoundManager {
private:
	static std::vector<Sound> sounds;

	//Volume between 0.0 and 1.0
	float masterVolume, musicVolume, effectVolume, ambientVolume, miscVolume;
public:
	void init(float masterVolume, float musicVolume, float effectVolume, float ambientVolume, float miscVolume);

	static void addSound(Sound &sound, SoundType type);

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