#pragma once

#include "../../../include/OpenAL/al.h"
#include "../../../include/OpenAL/alc.h"

struct SoundContext {
	//Initialize sound
	static void initSoundContext();

	// Cleanup sound
	static void deleteContext();
private:
	static ALCcontext* context;
};