#include "Framebuffer.h"

#include "GL/glew.h"

#include "../../../Utils/Logger.h"


#define FRAMEBUFFER_RENDERBUFFER_NON_EXISTENT this->rbo == 0

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &this->id);
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->colorAttachments.reserve(7);
	this->depthAttachment = nullptr;
	this->rbo = 0;
}

Framebuffer::~Framebuffer()
{
	for (Texture* attachment : this->colorAttachments)
	{
		glDeleteTextures(1, &attachment->id);
		delete attachment;
	}

	if (this->depthAttachment != nullptr)
		this->deleteDepthTexture();
	else
		this->deleteRenderBuffer();

	glDeleteFramebuffers(1, &this->id);
	
}

Texture* Framebuffer::attachTexture(const GLuint & width, const GLuint & height, const AttachmentType& attachmentTYPE)
{
	Texture* tex = new Texture();
	
	glGenTextures(1, &tex->id);
	glBindTexture(GL_TEXTURE_2D, tex->id);

	switch (attachmentTYPE) {
	case AttachmentType::COLOR:
		if (this->colorAttachments.size() < 7) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + this->colorAttachments.size(), GL_TEXTURE_2D, tex->id, 0);

			this->colorAttachments.push_back(tex);
		}
		else {
			delete tex;
			LOG_WARNING("You have exceeded the maxmimum amount of color attachments!");
		}
		break;

	case AttachmentType::DEPTH:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);

		if (this->depthAttachment != nullptr) {
			LOG_INFO("Old depth texture attachment has been replaced!");
			this->deleteDepthTexture();
		}
		if (this->rbo != 0) {
			LOG_INFO("Old renderbuffer depth attachment has been replaced with texture attachment!");
			this->deleteRenderBuffer();
		}


		this->depthAttachment = tex;
		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void Framebuffer::attachRenderBuffer(const GLuint & width, const GLuint & height)
{
	if (FRAMEBUFFER_RENDERBUFFER_NON_EXISTENT) {

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	else {
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, width, height);
	}
	if (this->depthAttachment != nullptr) {
		this->deleteDepthTexture();
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::updateDimensions(unsigned index, const GLuint & width, const GLuint & height)
{
	if (index < this->colorAttachments.size() - 1) {
		Texture* tex = this->colorAttachments.at(index);
		glBindTexture(GL_TEXTURE_2D, tex->id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else {
		LOG_ERROR("Index out of range, cannot update dimensions of non existent color attachment!");
	}

}

void Framebuffer::bind() const
{

	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		LOG_ERROR("Framebuffer is missing one of four requirements!");
		LOG_INFO("attach at least one buffer(color, depth or stencil buffer).");
		LOG_INFO("There should be at least one color attachment.");
		LOG_INFO("attachments should be complete as well (reserved memory).");
		LOG_INFO("Each buffer should have the same number of samples.");
	}
		
	
}

void Framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::deleteDepthTexture()
{
	glDeleteTextures(1, &this->depthAttachment->id);
	delete this->depthAttachment;
	this->depthAttachment = nullptr;
}

void Framebuffer::deleteRenderBuffer()
{
	glDeleteRenderbuffers(1, &this->rbo);
	this->rbo = 0;
}
