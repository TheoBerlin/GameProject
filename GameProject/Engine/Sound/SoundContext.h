#pragma once

#include "../../../include/OpenAL/al.h"
#include "../../../include/OpenAL/alc.h"

class SoundContext {
private:

public:
	SoundContext();
	~SoundContext();

	//Initialize sound
	static void initSoundContext();

};