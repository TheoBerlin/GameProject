#pragma once

#include <fstream>

#include "glm/glm.hpp"
#include "../../../include/OpenAL/al.h"
#include "../../../include/OpenAL/alc.h"

#pragma warning(disable:4996)
enum soundType {SOUND_MUSIC = 0, SOUND_EFFECT = 1, SOUND_AMBIENT = 2, SOUND_VOICE = 3, SOUND_MISC = 4};

class Sound {
private:
	ALuint source;
	ALuint buffer;

	bool errorCheck();
public:
	//Default pitch = 1, volume = 1, position = (0, 0, 0), velocity = (0, 0, 0), loop = false
	Sound(float pitch = 1.0f, float volume = 1.0f, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 velocity = glm::vec3(0, 0, 0), bool loop = false);
	~Sound();

	void loadSound(std::string file);
	void playSound();

	void setListener(const glm::vec3 listener);
	glm::vec3 getListener() const;
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
};