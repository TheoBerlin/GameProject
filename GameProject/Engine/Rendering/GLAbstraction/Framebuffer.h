#pragma once

#include "RenderingResources.h"

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
	Texture* attachTexture(const GLuint& width, const GLuint& height, const AttachmentType& type = AttachmentType::COLOR);
	/*
		Explicitly used as depth attachment replaces current depth attachment(Texture or RBO) with renderbuffer depth attachment
	*/
	void attachRenderBuffer(const GLuint& width, const GLuint& height);
	/*
		Updates the width and height of color attachment
	*/
	void updateDimensions(unsigned index, const GLuint& width, const GLuint& height);

	void bind() const;
	void unbind() const;

private:
	unsigned id, rbo;

	std::vector<Texture*> colorAttachments;
	Texture* depthAttachment;

	void deleteDepthTexture();
	void deleteRenderBuffer();

};
