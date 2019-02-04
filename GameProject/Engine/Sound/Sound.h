#pragma once

#include <fstream>

#include "glm/glm.hpp"
#include "../../Utils/Logger.h"
#include "../../../include/OpenAL/al.h"
#include "../../../include/OpenAL/alc.h"

#pragma warning(disable:4996)

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

	//Get the source. Used for changing multiple sources at same time.
	ALuint getSource();

	void setPitch(const float pitch);
	float getPitch() const;
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