#include "Sound.h"

#include <Windows.h>

#include "Utils/Logger.h"
#include "Engine/Sound/SoundManager.h"

bool Sound::errorCheck()
{
	bool error = true;
	ALCenum e;

	e = alGetError();
	if (e != AL_NO_ERROR) {
		std::cout << "OpenAL error with error code: " << e << std::endl;
		error = false;
	}

	return error;
}

Sound::Sound(float pitch, float volume, glm::vec3 position, glm::vec3 velocity, bool loop)
{
	alGenSources((ALuint)1, &source);
	errorCheck();

	alSourcef(source, AL_PITCH, pitch);
	errorCheck();
	this->volume = volume;
	alSourcef(source, AL_GAIN, volume);
	errorCheck();
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	errorCheck();
	alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	errorCheck();
	alSourcei(source, AL_LOOPING, loop);
	errorCheck();
	alDistanceModel(AL_LINEAR_DISTANCE);
	errorCheck();

	alGenBuffers((ALuint)1, &buffer);
	errorCheck();
}

Sound::~Sound()
{
	alDeleteBuffers(1, &buffer);
	alDeleteSources(1, &source);
}

void Sound::loadSound(std::string fileName)
{
	FILE *file;
	file = fopen(fileName.c_str(), "rb");
	
	if (file == NULL) {
		LOG_ERROR("SOUND: File not read properly");
		return;
	}
	//Read file size
	fseek(file, 0, SEEK_END);
	unsigned int size = ftell(file) - 44;

	//Read number of audio channels
	fseek(file, 22, SEEK_SET);
	fread(&channels, 2, 1, file);

	//Read audio frequezy
	fseek(file, 24, SEEK_SET);
	fread(&freq, 4, 1, file);

	//Read bits per sample
	fseek(file, 34, SEEK_SET);
	fread(&bitsPerSample, 2, 1, file);

	//Read audio data
	unsigned char* buf = new unsigned char[size];
	fseek(file, 44, SEEK_SET);
	fread(buf, sizeof(BYTE), size - 44, file);

	//Read data to buffer, (-44) is sice of wav header.
	if(channels == 1 && bitsPerSample == 8)
		alBufferData(buffer, AL_FORMAT_MONO8, buf, size, freq);
	if (channels == 1 && bitsPerSample == 16)
		alBufferData(buffer, AL_FORMAT_MONO16, buf, size, freq);
	if (channels == 2)
		LOG_ERROR("OpenAL can't play stereo sound, convert it to Mono");
	errorCheck();

	alSourcei(source, AL_BUFFER, buffer);
	errorCheck();

	delete buf;
}

void Sound::playSound()
{
	alSourcePlay(source);
	errorCheck();
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
	alSourcef(source, AL_PITCH, pitch);
	errorCheck();
}

float Sound::getPitch() const
{
	float ret;
	alGetSourcef(source, AL_PITCH, &ret);
	return ret;
}

void Sound::setVolume(const float volume)
{
	this->volume = volume;
	if (type == SOUND_AMBIENT)
		updateSound(SoundManager::get().getAmbientVolume() * SoundManager::get().getMasterVolume());
	if (type == SOUND_EFFECT)
		updateSound(SoundManager::get().getEffectVolume() * SoundManager::get().getMasterVolume());
	if (type == SOUND_MISC)
		updateSound(SoundManager::get().getMiscVolume() * SoundManager::get().getMasterVolume());
	if (type == SOUND_MUSIC)
		updateSound(SoundManager::get().getMusicVolume() * SoundManager::get().getMasterVolume());

	errorCheck();
}

float Sound::getVolume() const
{
	float ret;
	alGetSourcef(source, AL_GAIN, &ret);
	return ret;
}

void Sound::setPosition(const glm::vec3 position)
{
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	errorCheck();
}

glm::vec3 Sound::getPosition() const
{
	glm::vec3 ret;
	alGetSource3f(source, AL_POSITION, &ret.x, &ret.y, &ret.z);
	return ret;
}

void Sound::setVelocity(const glm::vec3 position)
{
	alSource3f(source, AL_VELOCITY, position.x, position.y, position.z);
	errorCheck();
}

glm::vec3 Sound::getVelocity() const
{
	glm::vec3 ret;
	alGetSource3f(source, AL_VELOCITY, &ret.x, &ret.y, &ret.z);
	return ret;
}

void Sound::setLoopState(const bool loop)
{
	alSourcei(source, AL_LOOPING, loop);
	errorCheck();
}

bool Sound::getLoopState() const
{
	ALint ret;
	alGetSourcei(source, AL_LOOPING, &ret);
	if (ret == 1)
		return true;
	if (ret == 0)
		return false;
	return false;
}

void Sound::setSourceRelative(const bool relative)
{
	alSourcei(source, AL_SOURCE_RELATIVE, relative);
	errorCheck();
}

bool Sound::getSourceRelative() const
{
	ALint ret;
	alGetSourcei(source, AL_SOURCE_RELATIVE, &ret);
	if (ret == 1)
		return true;
	if (ret == 0)
		return false;
	return false;
}

void Sound::updateSound(float volume)
{
	alSourcef(source, AL_GAIN, this->volume * volume);
	errorCheck();
}
