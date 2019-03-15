#include "SoundManager.h"
#include <Utils/Logger.h>
#include "SoundConfig.h"

SoundManager & SoundManager::get()
{
	static SoundManager soundManager;
	return soundManager;
}

void SoundManager::addSound(Sound* sound, SoundType type)
{
	sound->setSoundType(type);
	sounds.push_back(sound);
	switch (sound->getSoundType()) {
	case(SOUND_MUSIC): sound->updateSound(masterVolume * musicVolume); break;
	case(SOUND_EFFECT): sound->updateSound(masterVolume * effectVolume); break;
	case(SOUND_AMBIENT): sound->updateSound(masterVolume * ambientVolume); break;
	case(SOUND_MISC): sound->updateSound(masterVolume * miscVolume); break;
	}
}

void SoundManager::removeSound(Sound * sound)
{
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sound == sounds[i]) {
			sounds.erase(sounds.begin() + i);
		}
	}
}

std::vector<Sound*>* SoundManager::getSoundsVector()
{
	return &sounds;
}

void SoundManager::setListenerPosition(glm::vec3 position)
{
	AL_CALL(alListener3f(AL_POSITION, position.x, position.y, position.z));
}

glm::vec3 SoundManager::getListenerPosition() const
{
	glm::vec3 position;
	AL_CALL(alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z));
	return position;
}

void SoundManager::setListenerOrientation(glm::vec3 at, glm::vec3 up)
{
	ALfloat orientation[6] = { at.x, at.y, at.z, up.x, up.y, up.z };
	AL_CALL(alListenerfv(AL_ORIENTATION, orientation));
}

glm::vec3 SoundManager::getListenerOrientation() const
{
	glm::vec3 orientation;
	AL_CALL(alGetListener3f(AL_ORIENTATION, &orientation.x, &orientation.y, &orientation.z));
	return orientation;
}

void SoundManager::setMasterVolume(float volume)
{
	this->masterVolume = glm::clamp(volume, 0.0f, 1.0f);

	for (unsigned int i = 0; i < sounds.size(); i++) {
		switch (sounds[i]->getSoundType()) {
		case(SOUND_MUSIC): sounds[i]->updateSound(masterVolume * musicVolume); break;
		case(SOUND_EFFECT): sounds[i]->updateSound(masterVolume * effectVolume); break;
		case(SOUND_AMBIENT): sounds[i]->updateSound(masterVolume * ambientVolume); break;
		case(SOUND_MISC): sounds[i]->updateSound(masterVolume * miscVolume); break;
		}
	}
}

float SoundManager::getMasterVolume() const
{
	return masterVolume;
}

void SoundManager::setMusicVolume(float volume)
{
	musicVolume = glm::clamp(volume, 0.0f, 1.0f);

	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getSoundType() == SOUND_MUSIC)
			sounds[i]->updateSound(masterVolume * musicVolume);
	}
}

float SoundManager::getMusicVolume() const
{
	return musicVolume;
}

void SoundManager::setEffectVolume(float volume)
{
	effectVolume = glm::clamp(volume, 0.0f, 1.0f);

	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getSoundType() == SOUND_EFFECT)
			sounds[i]->updateSound(masterVolume * effectVolume);
	}
}

float SoundManager::getEffectVolume() const
{
	return effectVolume;
}

void SoundManager::setEffectPitch(float pitch)
{
	pitch = glm::min(pitch, 0.0f);

	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getSoundType() == SOUND_EFFECT)
			sounds[i]->setPitch(pitch);
	}
}

void SoundManager::offsetEffects(float seconds)
{
	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getSoundType() == SOUND_EFFECT)
			sounds[i]->offsetPlayTime(seconds);
	}
}

void SoundManager::setAmbientVolume(float volume)
{
	ambientVolume = glm::clamp(volume, 0.0f, 1.0f);

	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getSoundType() == SOUND_AMBIENT)
			sounds[i]->updateSound(masterVolume * ambientVolume);
	}
}

float SoundManager::getAmbientVolume() const
{
	return ambientVolume;
}

void SoundManager::setMiscVolume(float volume)
{
	miscVolume = glm::clamp(volume, 0.0f, 1.0f);

	for (unsigned int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getSoundType() == SOUND_MISC)
			sounds[i]->updateSound(masterVolume * miscVolume);
	}
}

float SoundManager::getMiscVolume() const
{
	return miscVolume;
}
