#include "SoundContext.h"

#include "Utils/Logger.h"

void SoundContext::initSoundContext()
{
	ALCdevice *device;

	device = alcOpenDevice(NULL);
	if (!device)
		LOG_ERROR("OpenAL could not find device");

	ALboolean enumeration;

	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
		LOG_ERROR("OpenAL enumeration not supported");
	else
		LOG_SUCCESS("OpenAL enumeration supported");

	ALCcontext *context;
	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context))
		LOG_ERROR("OpenAL context failed to load");
}
