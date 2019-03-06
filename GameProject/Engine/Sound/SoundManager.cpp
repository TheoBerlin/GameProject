#include "SoundManager.h"


bool SoundManager::errorCheck()
{
	bool error = true;
	ALCenum e;

	e = alGetError();
	if (e != AL_NO_ERROR) {
		LOG_ERROR("OpenAL error with error code: %s", e);
		error = false;
	}

	return error;
}

SoundManager & SoundManager::get()
{
	static SoundManager soundManager;
	return soundManager;
}

void SoundManager::addSound(Sound & sound, SoundType type)
{
	sound.setSoundType(type);
	sounds.push_back(sound);
	switch (sound.getSoundType()) {
	case(SOUND_MUSIC): sound.updateSound(masterVolume * musicVolume); break;
	case(SOUND_EFFECT): sound.updateSound(masterVolume * effectVolume); break;
	case(SOUND_AMBIENT): sound.updateSound(masterVolume * ambientVolume); break;
	case(SOUND_MISC): sound.updateSound(masterVolume * miscVolume); break;
	}
}

void SoundManager::setListenerPosition(glm::vec3 position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	errorCheck();
}

glm::vec3 SoundManager::getListenerPosition() const
{
	glm::vec3 position;
	alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);
	return position;
}

void SoundManager::setListenerOrientation(glm::vec3 at, glm::vec3 up)
{
	ALfloat orientation[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
	alListenerfv(AL_ORIENTATION, orientation);
	errorCheck();
}

glm::vec3 SoundManager::getListenerOrientation() const
{
	glm::vec3 orientation;
	alGetListener3f(AL_ORIENTATION, &orientation.x, &orientation.y, &orientation.z);
	return orientation;
}

void SoundManager::setMasterVolume(float volume)
{
	this->masterVolume = volume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		switch (sounds[i].getSoundType()) {
		case(SOUND_MUSIC): sounds[i].updateSound(masterVolume * musicVolume); break;
		case(SOUND_EFFECT): sounds[i].updateSound(masterVolume * effectVolume); break;
		case(SOUND_AMBIENT): sounds[i].updateSound(masterVolume * ambientVolume); break;
		case(SOUND_MISC): sounds[i].updateSound(masterVolume * miscVolume); break;
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
			sounds[i].updateSound(masterVolume * musicVolume);
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
			sounds[i].updateSound(masterVolume * effectVolume);
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
			sounds[i].updateSound(masterVolume * ambientVolume);
	}
}

float SoundManager::getAmbientVolume() const
{
	return ambientVolume;
}

void SoundManager::setMiscVolume(float volume)
{
	miscVolume = volume;
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i].getSoundType() == SOUND_MISC)
			sounds[i].updateSound(masterVolume * miscVolume);
	}
}

float SoundManager::getMiscVolume() const
{
	return miscVolume;
}
