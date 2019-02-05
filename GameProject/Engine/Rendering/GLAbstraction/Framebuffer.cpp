#include "Framebuffer.h"

#include "GL/glew.h"

#include "../../../Utils/Logger.h"

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &this->id);
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
}

void Framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
