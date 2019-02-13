#include "GUIRenderer.h"

#include "../GUI/FontManager.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Utils/Logger.h"

GUIRenderer::GUIRenderer()
{
	this->shader = new Shader("./Engine/Rendering/Shaders/TexShader.vert", "./Engine/Rendering/Shaders/TexShader.frag");
	initTextRendering();
}

GUIRenderer::~GUIRenderer()
{
	delete this->va;
	delete this->vaQuad;
	delete this->shader;
}

void GUIRenderer::bakeText(Text & text, float scale)
{
	Display& display = Display::get();

	if (scale > 0.0f)
		text.setScale(scale);
	float currentScale = scale > 0.0f ? scale : text.getScale();

	fb.updateDimensions(0, text.getWidth(), text.getHeight());
	fb.bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	drawToBake(text, currentScale);
	fb.unbind();
	glViewport(0, 0, display.getWidth(), display.getHeight());
	text.setBakedTexture(fb.getColorTexture(0));
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawToBake(Text & text, float scale)
{
	Font* font = text.getFont();

	Display& display = Display::get();

	float sx = 1.0f / (float)(text.getWidth());
	float sy = 1.0f / (float)(text.getHeight());
	const float scaleX = sx * 2.0f;
	const float scaleY = sy * 2.0f;
	float x = -1.0f;
	float y = -1.0f + (text.getHeight()-text.getBearingY())*scaleY;

	std::vector<Text::CharacterDrawData> charactersData = text.getCharactersDrawData();
	for (unsigned int i = 0; i < charactersData.size(); i++)
	{
		Text::CharacterDrawData character = charactersData[i];

		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, character.textureID);
		font->getShader()->setUniform2f("scale", character.scale.x * scaleX, character.scale.y * scaleY);
		font->getShader()->setUniform2f("pos", character.pos.x * scaleX + x, character.pos.y * scaleY + y);

		this->va->bind();
		this->vb->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void GUIRenderer::drawBaked(Text & text, float x, float y)
{
	Display& display = Display::get();

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();

	float scaleX = sx * text.getScale();
	float scaleY = sy * text.getScale();

	this->shader->bind();
	this->shader->setTexture2D("tex", 0, *text.getBakedTexture());
	this->shader->setUniform2f("pos", x + text.getWidth()*scaleX*0.5f, y + text.getHeight()*scaleY*0.5f);
	this->shader->setUniform2f("scale", text.getWidth()*scaleX*0.5f, text.getHeight()*scaleY*0.5f);
	this->vaQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->shader->unbind();
}

void GUIRenderer::draw(Text & text, float x, float y, float scale)
{
	Font* font = text.getFont();
	Display& display = Display::get();

	float currentScale = scale < 0.0f ? text.getScale() : scale;

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();
	const float scaleX = sx * currentScale;
	const float scaleY = sy * currentScale;

	std::vector<Text::CharacterDrawData> charactersData = text.getCharactersDrawData();
	for (unsigned int i = 0; i < charactersData.size(); i++)
	{
		Text::CharacterDrawData character = charactersData[i];

		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, character.textureID);
		font->getShader()->setUniform2f("scale", character.scale.x * scaleX, character.scale.y * scaleY);
		font->getShader()->setUniform2f("pos", character.pos.x * scaleX + x, character.pos.y * scaleY + y);
		
		this->va->bind();
		this->vb->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void GUIRenderer::draw(const std::string & str, float x, float y, const glm::vec4& color, const std::string& fontName, float scale)
{
	Font* font = FontManager::getFont(fontName);
	Display& display = Display::get();

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();

	const char* p;
	for (p = str.c_str(); *p; p++)
	{
		Font::Character character = font->getCharacter(*p);

		float x2 = x + character.bearingX * sx * scale;
		float y2 = y - (character.height - character.bearingY) * sy * scale;
		float w = character.width * sx * scale;
		float h = character.height * sy * scale;

		GLfloat rect[4][4] = {
			{ x2	, y2 + h, 0, 0 }, // TL
			{ x2	, y2	, 0, 1 }, // BL
			{ x2 + w, y2 + h, 1, 0 }, // TR
			{ x2 + w, y2	, 1, 1 }, // BR
		};
		this->vb->updateData(rect, sizeof(rect));
		
		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &(color.x));
		font->getShader()->setTexture2D("tex", 0, character.textureID);

		this->va->bind();
		this->vb->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (character.advance >> 6) * sx * scale * 1.1f;
	}
}

void GUIRenderer::prepareTextRendering()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GUIRenderer::initTextRendering()
{
	// Vao for each character
	this->va = new VertexArray();
	GLfloat rect2[4][4] = {
			{ -0.5f, +0.5f, 0, 0 }, // TL
			{ -0.5f, -0.5f, 0, 1 }, // BL
			{ +0.5f, +0.5f, 1, 0 }, // TR
			{ +0.5f, -0.5f, 1, 1 }, // BR
	};
	this->vb = new VertexBuffer(rect2, sizeof(rect2), GL_STATIC_DRAW);
	AttributeLayout layout;
	layout.push(4); // [vec4] Position and uv.
	this->va->addBuffer(vb, layout);

	// Vao for baked texture.
	this->vaQuad = new VertexArray();
	GLfloat rect[4][4] = {
			{ -1.0f, +1.0f, 0, 0 }, // TL
			{ -1.0f, -1.0f, 0, 1 }, // BL
			{ +1.0f, +1.0f, 1, 0 }, // TR
			{ +1.0f, -1.0f, 1, 1 }, // BR
	};
	this->vbQuad = new VertexBuffer(rect, sizeof(rect), GL_STATIC_DRAW);
	this->vaQuad->addBuffer(vbQuad, layout);

	Display& display = Display::get();
	this->fb.attachTexture(display.getWidth(), display.getHeight(), AttachmentType::COLOR);
}
