#pragma once

#include "Texture.h"

enum AttachmentType {
	COLOR,
	DEPTH
};
class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();
	/*
		Used to attach texture attachment on framebuffer, creates buffer with requested dimensions and type and leaves it empty.
	*/
	Texture* attachTexture(const GLuint& width, const GLuint& height, const AttachmentType& type = AttachmentType::COLOR, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA);
	/*
		Explicitly used as depth attachment replaces current depth attachment(Texture or RBO) with renderbuffer depth attachment
	*/
	void attachRenderBuffer(const GLuint& width, const GLuint& height);

	/*
		Updates the width and height of color attachment and depth attachments if it exists.
	*/
	void updateDimensions(unsigned index, const GLuint& width, const GLuint& height, unsigned internalFormat = GL_RGBA, unsigned format = GL_RGBA);

	/*
		Get Color Texture attachment from index, in order of attachment.
	*/
	Texture* getColorTexture(unsigned index);
	/*
		Returns depth texture if it exists, otherwise returns nullptr
	*/
	Texture* getDepthTexture();


	void bind() const;
	void unbind() const;

private:
	static unsigned currentBindedId;
	unsigned id, rbo;

	std::vector<Texture*> colorAttachments;
	Texture* depthAttachment;

	void deleteDepthTexture();
	void deleteRenderBuffer();
};
