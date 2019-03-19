#pragma once

#include <fstream>

#include "glm/glm.hpp"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#pragma warning(disable:4996)
enum SoundType {SOUND_MUSIC = 0, SOUND_EFFECT = 1, SOUND_AMBIENT = 2, SOUND_MISC = 4};

class Sound {
private:
	ALuint source;
	ALuint buffer;

	std::string fileName;

	SoundType type;

	float volume;
	// pitch = localPitch * masterPitch
	float localPitch;

	unsigned int freq;
	unsigned short int channels;
	unsigned short int bitsPerSample;

public:
	//Default pitch = 1, volume = 1, position = (0, 0, 0), velocity = (0, 0, 0), loop = false
	Sound(float pitch = 1.0f, float volume = 1.0f, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0), bool loop = false);
	~Sound();

	void loadSound(std::string file);
	// Start playing a sound or resume it.
	void playSound();
	// Pause the sound. If playSound is called after pausing it, the sound will resume playing.
	void pauseSound();
	// Stop the sound. If playSound is called after stopping it, the sound will start from the begining again.
	void stopSound();

	// Fast forward or rewind the time
	void offsetPlayTime(float seconds);

	void setSoundType(SoundType type);
	SoundType getSoundType() const;

	//Pitch multiplier should always be a positive value
	void setPitch(const float pitch);
	// Returns local pitch multiplier
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