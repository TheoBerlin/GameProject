#include "Pipeline.h"



Pipeline::Pipeline()
{


	this->fbo.attachTexture(512, 512, AttachmentType::COLOR);
}


Pipeline::~Pipeline()
{
}


Texture * Pipeline::drawToTexture()
{
	glEnable(GL_DEPTH_TEST);

	this->fbo.bind();
	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

void Pipeline::drawTextureToQuad(const Texture & tex)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
}
