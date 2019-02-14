#pragma once

#include "GL/glew.h"

namespace Formats
{
	unsigned int getTextureFormatChannels(GLuint format)
	{
		switch (format)
		{
		case GL_RGBA:		return 4;
		case GL_RGB:		return 3;
		case GL_RGBA16F:	return 4;
		case GL_RED:		return 1;
		default:			return 0;
		}
	}
}