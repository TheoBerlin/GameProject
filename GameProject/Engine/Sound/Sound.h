#pragma once

#include <fstream>

#include "glm/glm.hpp"
#include "../../../include/OpenAL/al.h"
#include "../../../include/OpenAL/alc.h"

#pragma warning(disable:4996)
enum SoundType {SOUND_MUSIC = 0, SOUND_EFFECT = 1, SOUND_AMBIENT = 2, SOUND_VOICE = 3, SOUND_MISC = 4};

class Sound {
private:
	ALuint source;
	ALuint buffer;

	SoundType type;
	float volume;

	unsigned int freq;
	unsigned short int channels;
	unsigned short int bitsPerSample;

	bool errorCheck();
public:
	//Default pitch = 1, volume = 1, position = (0, 0, 0), velocity = (0, 0, 0), loop = false
	Sound(float pitch = 1.0f, float volume = 1.0f, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0), bool loop = false);
	~Sound();

	void loadSound(std::string file);
	void playSound();

	void setSoundType(SoundType type);
	SoundType getSoundType() const;

	//Pitch multiplier should always positive value
	void setPitch(const float pitch);
	float getPitch() const;
	//Volume is between 0.0 and 1.0
	void setVolume(const float volume);
	float getVolume() const;
	void setPosition(const glm::vec3 position);
	glm::vec3 getPosition() const;
	void setVelocity(const glm::vec3 position);
	glm::vec3 getVelocity() const;
	//True = looping, False = not looping
	void setLoopState(const bool loop);
	bool getLoopState() const;
	//Sets the sounds source to be relavtive to listener
	void setSourceRelative(const bool relative);
	bool getSourceRelative() const;
	//Updates volume of sound without affecting volume variable. Should exclusivly be used by sound manager
	void updateSound(float volume);
};