#include "SoundContext.h"

#include "Utils/Logger.h"
#include "SoundConfig.h"

ALCcontext* SoundContext::context = nullptr;

void SoundContext::initSoundContext()
{
	ALCdevice *device;

	AL_CALL(device = alcOpenDevice(NULL));
	if (!device)
		LOG_ERROR("OpenAL could not find device");

	ALboolean enumeration;
	AL_CALL(enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"));
	if (enumeration == AL_FALSE)
		LOG_ERROR("OpenAL enumeration not supported");
	else
		LOG_SUCCESS("OpenAL enumeration supported");

	AL_CALL(context = alcCreateContext(device, NULL));
	if (context != NULL) {
		ALboolean contextCurrent = false;
		AL_CALL(contextCurrent = alcMakeContextCurrent(context));
		if (!contextCurrent)
			LOG_ERROR("OpenAL context failed to be current");
	}
	else {
		LOG_ERROR("OpenAL context failed to load");
	}
}

void SoundContext::deleteContext()
{
	ALCdevice *device;
	AL_CALL(device = alcGetContextsDevice(context));
	AL_CALL(alcMakeContextCurrent(NULL));
	AL_CALL(alcDestroyContext(context));
	AL_CALL(alcCloseDevice(device));
}
