#include "Sound.h"

#include <Windows.h>

#include "../../Utils/Logger.h"

bool Sound::errorCheck()
{
	ALCenum error;

	error = alGetError();
	if (error != AL_NO_ERROR)
		std::cout << error << std::endl;

	return false;
}

Sound::Sound(float pitch, float volume, glm::vec3 position, glm::vec3 velocity, bool loop)
{
	alGenSources((ALuint)1, &source);
	errorCheck();

	alSourcef(source, AL_PITCH, pitch);
	errorCheck();
	alSourcef(source, AL_GAIN, volume);
	errorCheck();
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	errorCheck();
	alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	errorCheck();
	alSourcei(source, AL_LOOPING, loop);
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
	unsigned int size = ftell(file);

	//Read audio frequezy
	fseek(file, 24, SEEK_SET);
	unsigned int freq;
	fread(&freq, 4, 1, file);

	//Read audio data
	unsigned char* buf = new unsigned char[size];
	fseek(file, 44, SEEK_SET);
	fread(buf, sizeof(BYTE), size - 44, file);

	//Read data to buffer, (-44) is sice of wav header.
	alBufferData(buffer, AL_FORMAT_MONO16, buf, size - 44, freq);
	errorCheck();

	alSourcei(source, AL_BUFFER, buffer);
	errorCheck();

	delete buf;
}

void Sound::playSound()
{
	alSourcePlay(source);
}

void Sound::setListener(const glm::vec3 listener)
{
	alListener3f(source, listener.x, listener.y, listener.z);
}

glm::vec3 Sound::getListener() const
{
	glm::vec3 position;
	alGetListener3f(source, &position.x, &position.y, &position.z);
	return position;
}

void Sound::setPitch(const float pitch)
{
	alSourcef(source, AL_PITCH, pitch);
}

float Sound::getPitch() const
{
	float ret;
	alGetSourcef(source, AL_PITCH, &ret);
	return ret;
}

void Sound::setVolume(const float volume)
{
	alSourcef(source, AL_GAIN, volume);
}

float Sound::getVolume() const
{
	float ret;
	alGetSourcef(source, AL_GAIN, &ret);
	return ret;
}

void Sound::setPosition(const glm::vec3 position)
{
	alSource3f(source, AL_GAIN, position.x, position.y, position.z);
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
