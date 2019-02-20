#include "SoundManager.h"

void SoundManager::init(float masterVolume, float musicVolume, float effectVolume, float ambientVolume, float miscVolume)
{
	this->masterVolume = masterVolume;
	this->musicVolume = musicVolume;
	this->effectVolume = effectVolume;
	this->ambientVolume = ambientVolume;
	this->miscVolume = miscVolume;
}

void SoundManager::addSound(Sound & sound, SoundType type)
{
	sound.setSoundType(type);
	sounds.push_back(sound);
}

void SoundManager::setMasterVolume(float volume)
{
	this->masterVolume = volume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		switch (sounds[i].getSoundType()) {
		case(SOUND_MUSIC): sounds[i].setVolume(masterVolume * musicVolume); break;
		case(SOUND_EFFECT): sounds[i].setVolume(masterVolume * effectVolume); break;
		case(SOUND_AMBIENT): sounds[i].setVolume(masterVolume * ambientVolume); break;
		case(SOUND_MISC): sounds[i].setVolume(masterVolume * miscVolume); break;
		}
	}
}

float SoundManager::getMasterVolume() const
{
	return masterVolume;
}

void SoundManager::setMusicVolume(float volume)
{
	musicVolume = volume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i].getSoundType() == SOUND_MUSIC)
			sounds[i].setVolume(masterVolume * musicVolume);
	}
}

float SoundManager::getMusicVolume() const
{
	return musicVolume;
}

void SoundManager::setEffectVolume(float volume)
{
	effectVolume = volume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i].getSoundType() == SOUND_EFFECT)
			sounds[i].setVolume(masterVolume * effectVolume);
	}
}

float SoundManager::getEffectVolume() const
{
	return effectVolume;
}

void SoundManager::setAmbientVolume(float volume)
{
	ambientVolume = volume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i].getSoundType() == SOUND_AMBIENT)
			sounds[i].setVolume(masterVolume * ambientVolume);
	}
}

float SoundManager::getAmbientVolume() const
{
	return ambientVolume;
}

void SoundManager::setMiscVolume(float volume)
{
	miscVolume = volume;
}

float SoundManager::getMiscVolume() const
{
	return musicVolume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i].getSoundType() == SOUND_MISC)
			sounds[i].setVolume(masterVolume * miscVolume);
	}
}
