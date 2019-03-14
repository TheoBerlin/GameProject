#pragma once

/*
This will process the statement f and then check for errors.
*/
#define AL_CALL(f) \
		{ \
		f; \
		ALCenum e = alGetError(); \
		if (e != AL_NO_ERROR) \
			LOG_ERROR("OpenAL error with error code: %s", alGetString(e)); \
		}