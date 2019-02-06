#include "Framebuffer.h"

#include "GL/glew.h"

#include "../../../Utils/Logger.h"


#define FRAMEBUFFER_RENDERBUFFER_NON_EXISTENT this->rbo == 0

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &this->id);


	this->colorAttachments.reserve(7);
	this->depthAttachment = nullptr;
	this->rbo = 0;
}

Framebuffer::~Framebuffer()
{
	for (Texture* attachment : this->colorAttachments)
		delete attachment;

	if (this->depthAttachment != nullptr)
		this->deleteDepthTexture();
	else
		this->deleteRenderBuffer();

	glDeleteFramebuffers(1, &this->id);
}

Texture* Framebuffer::attachTexture(const GLuint & width, const GLuint & height, const AttachmentType& attachmentTYPE)
{
	Texture* tex = new Texture();
	
	switch (attachmentTYPE) {
	case AttachmentType::COLOR:
	{
		if (this->colorAttachments.size() < 7) {
			tex->recreate(NULL, width, height);

			glBindFramebuffer(GL_FRAMEBUFFER, this->id);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + this->colorAttachments.size(), GL_TEXTURE_2D, tex->getID(), 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			this->colorAttachments.push_back(tex);
		}
		else {
			delete tex;
			LOG_WARNING("You have exceeded the maxmimum amount of color attachments!");
		}
	}
		break;

	case AttachmentType::DEPTH:
	{
		tex->recreate(NULL, width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);

		glBindFramebuffer(GL_FRAMEBUFFER, this->id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->getID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (this->depthAttachment != nullptr) {
			LOG_INFO("Old depth texture attachment has been replaced!");
			this->deleteDepthTexture();
		}
		if (this->rbo != 0) {
			LOG_INFO("Old renderbuffer depth attachment has been replaced with texture attachment!");
			this->deleteRenderBuffer();
		}


		this->depthAttachment = tex;
	}
		break;
	}

	tex->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return tex;
}

void Framebuffer::attachRenderBuffer(const GLuint & width, const GLuint & height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::updateDimensions(unsigned index, const GLuint & width, const GLuint & height)
{
	if (this->depthAttachment != nullptr) {
		this->depthAttachment->resize(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	}


	if (index >= 0 && index <= this->colorAttachments.size() - 1) {
		Texture* tex = this->colorAttachments.at(index);
		tex->resize(width, height);
	}
	else {
		LOG_WARNING("Index out of range, cannot update dimensions of non existent color attachment!");
	}
}

Texture * Framebuffer::getColorTexture(unsigned index)
{
	if (index <= this->colorAttachments.size() - 1)
		return this->colorAttachments[index];
	else {
		LOG_WARNING("Index out of range!");
		return nullptr;
	}
}

Texture * Framebuffer::getDepthTexture()
{
	return this->depthAttachment;
}

void Framebuffer::bind() const
{
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		LOG_ERROR("Framebuffer is missing one of four requirements!");
		LOG_INFO("attach at least one buffer(color, depth or stencil buffer).");
		LOG_INFO("There should be at least one color attachment.");
		LOG_INFO("attachments should be complete as well (reserved memory).");
		LOG_INFO("Each buffer should have the same number of samples.");
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, this->id);
	}
		
	
}

void Framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::deleteDepthTexture()
{
	delete this->depthAttachment;
	this->depthAttachment = nullptr;
}

void Framebuffer::deleteRenderBuffer()
{
	glDeleteRenderbuffers(1, &this->rbo);
	this->rbo = 0;
}
