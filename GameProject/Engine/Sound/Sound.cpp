#include "Sound.h"

#include <Windows.h>
#include <Utils/Logger.h>

#include "Utils/Logger.h"
#include "Engine/Sound/SoundManager.h"
#include "SoundConfig.h"

Sound::Sound(float pitch, float volume, glm::vec3 position, glm::vec3 velocity, bool loop)
{
	AL_CALL(alGenSources((ALsizei)1, &source));

	this->volume = volume;
	AL_CALL(alSourcef(source, AL_GAIN, volume));

	this->localPitch = pitch;
	AL_CALL(alSourcef(source, AL_PITCH, pitch));

	AL_CALL(alSource3f(source, AL_POSITION, position.x, position.y, position.z));
	AL_CALL(alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z));
	AL_CALL(alSourcei(source, AL_LOOPING, loop));
	AL_CALL(alDistanceModel(AL_LINEAR_DISTANCE));

	AL_CALL(alGenBuffers((ALuint)1, &buffer));
}

Sound::~Sound()
{
	stopSound();
	AL_CALL(alDeleteSources((ALsizei)1, &source));
	AL_CALL(alDeleteBuffers((ALsizei)1, &buffer));
}

void Sound::loadSound(std::string fileName)
{
	FILE *file;
	file = fopen(fileName.c_str(), "rb");
	if (file == NULL) {
		LOG_ERROR("SOUND: File not read properly");
		return;
	}

	this->fileName = fileName;

	//Read number of audio channels
	fseek(file, 22, SEEK_SET);
	fread(&channels, 2, 1, file);

	//Read audio frequezy
	fseek(file, 24, SEEK_SET);
	fread(&freq, 4, 1, file);

	//Read bits per sample
	fseek(file, 34, SEEK_SET);
	fread(&bitsPerSample, 2, 1, file);

	//Read bits per sample
	unsigned int size;
	fseek(file, 40, SEEK_SET);
	fread(&size, 4, 1, file);

	//Read audio data
	unsigned char* buf = new unsigned char[size];
	fseek(file, 44, SEEK_SET);
	fread(buf, sizeof(BYTE), size, file);

	//Read data to buffer, (-44) is sice of wav header.
	if (channels == 1 && bitsPerSample == 8) {
		AL_CALL(alBufferData(buffer, AL_FORMAT_MONO8, buf, size, freq));
	}
	else if (channels == 1 && bitsPerSample == 16) {
		AL_CALL(alBufferData(buffer, AL_FORMAT_MONO16, buf, size, freq));
	}
	else if (channels == 2 && bitsPerSample == 8) {
		LOG_ERROR("OpenAL can't play stereo 16 bit, convert it to Mono");
	}
	else if (channels == 2 && bitsPerSample == 16) {
		unsigned int newSize = size / 2;
		for (unsigned i = 0; i < size; i += 4) {
			short left = *(short*)&buf[i];
			short right = *(short*)&buf[i + 2];
			short monoSample = (int(left) + right) / 2;
			buf[i / 2] = (char)monoSample;
			buf[i / 2 + 1] = (char)(monoSample >> 8);
		}
		AL_CALL(alBufferData(buffer, AL_FORMAT_MONO16, buf, newSize, freq));
	}

	AL_CALL(alSourcei(source, AL_BUFFER, buffer));

	delete buf;
}

void Sound::playSound()
{
	ALint playing = 0;
	AL_CALL(alGetSourcei(source, AL_SOURCE_STATE, &playing));
	
	if (playing == AL_INITIAL || playing == AL_STOPPED || playing == AL_PAUSED) {
		AL_CALL(alSourcePlay(source));
	}
}

void Sound::pauseSound()
{
	ALint playing = 0;
	AL_CALL(alGetSourcei(source, AL_SOURCE_STATE, &playing));

	if (playing == AL_PLAYING) {
		AL_CALL(alSourceStop(source));
	}
}

void Sound::stopSound()
{
	ALint playing = 0;
	AL_CALL(alGetSourcei(source, AL_SOURCE_STATE, &playing));

	if (playing == AL_PLAYING || playing == AL_PAUSED) {
		AL_CALL(alSourceStop(source));
	}
}

void Sound::offsetPlayTime(float seconds)
{
	ALint isPlaying = 0;
	AL_CALL(alGetSourcei(source, AL_SOURCE_STATE, &isPlaying));

	if (isPlaying != AL_PLAYING) {
		return;
	}

	// Get the current play time
	float currentTime;

	alGetSourcef(source, AL_SEC_OFFSET, &currentTime);

	float newTime = seconds + currentTime;

	// Check if the new time is larger than the length of the sound
	int byteSize;

	alGetBufferi(buffer, AL_SIZE, &byteSize);

	unsigned int sampleLength = byteSize * 8 / (channels * bitsPerSample);

	float secondLength = (float)sampleLength / (float)freq;

	if (secondLength < newTime) {
		// The new time is larger than the total sound length
		// Check if the sound if looping, if not, stop playing the sound
		ALint loopState;
		AL_CALL(alGetSourcei(source, AL_LOOPING, &loopState));

		if (loopState == AL_LOOPING) {
			alSourcef(source, AL_SEC_OFFSET, newTime);
		} else {
			this->stopSound();
		}
	}

	else {
		alSourcef(source, AL_SEC_OFFSET, newTime);
	}
}

void Sound::setSoundType(SoundType type)
{
	this->type = type;
}

SoundType Sound::getSoundType() const
{
	return type;
}

void Sound::setPitch(const float pitch)
{
	float masterPitch = SoundManager::get().getEffectsMasterPitch();
	this->localPitch = pitch;

	AL_CALL(alSourcef(source, AL_PITCH, this->localPitch * masterPitch));
}

float Sound::getPitch() const
{
	return this->localPitch;
}

void Sound::setVolume(const float volume)
{
	this->volume = volume;
	if (type == SOUND_AMBIENT) {
		updateSound(SoundManager::get().getAmbientVolume() * SoundManager::get().getMasterVolume());
	}
	else if (type == SOUND_EFFECT) {
		updateSound(SoundManager::get().getEffectsVolume() * SoundManager::get().getMasterVolume());
	}
	else if (type == SOUND_MISC) {
		updateSound(SoundManager::get().getMiscVolume() * SoundManager::get().getMasterVolume());
	}
	else if (type == SOUND_MUSIC) {
		updateSound(SoundManager::get().getMusicVolume() * SoundManager::get().getMasterVolume());
	}
}

float Sound::getVolume() const
{
	float ret;
	AL_CALL(alGetSourcef(source, AL_GAIN, &ret));

	return ret;
}

void Sound::setPosition(const glm::vec3 position)
{
	AL_CALL(alSource3f(source, AL_POSITION, position.x, position.y, position.z));
}

glm::vec3 Sound::getPosition() const
{
	glm::vec3 ret;
	AL_CALL(alGetSource3f(source, AL_POSITION, &ret.x, &ret.y, &ret.z));
	return ret;
}

void Sound::setVelocity(const glm::vec3 position)
{
	AL_CALL(alSource3f(source, AL_VELOCITY, position.x, position.y, position.z));
}

glm::vec3 Sound::getVelocity() const
{
	glm::vec3 ret;
	AL_CALL(alGetSource3f(source, AL_VELOCITY, &ret.x, &ret.y, &ret.z));
	return ret;
}

void Sound::setLoopState(const bool loop)
{
	AL_CALL(alSourcei(source, AL_LOOPING, loop));
}

bool Sound::getLoopState() const
{
	ALint ret;
	AL_CALL(alGetSourcei(source, AL_LOOPING, &ret));
	if (ret == 1)
		return true;
	if (ret == 0)
		return false;
	return false;
}

void Sound::setSourceRelative(const bool relative)
{
	AL_CALL(alSourcei(source, AL_SOURCE_RELATIVE, relative));
}

bool Sound::getSourceRelative() const
{
	ALint ret;
	AL_CALL(alGetSourcei(source, AL_SOURCE_RELATIVE, &ret));
	if (ret == 1)
		return true;
	if (ret == 0)
		return false;
	return false;
}

void Sound::updateSound(float volume)
{
	AL_CALL(alSourcef(source, AL_GAIN, this->volume * volume));
}
