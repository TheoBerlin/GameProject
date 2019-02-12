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

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();
	fb.updateDimensions(0, text.getWidth(), text.getHeight());
	fb.bind();
	glClear(GL_COLOR_BUFFER_BIT);
	drawToBake(text, scale);
	fb.unbind();
	text.setTexture(fb.getColorTexture(0));
}

void GUIRenderer::drawToBake(Text & text, float scale)
{
	Font* font = text.getFont();

	Display& display = Display::get();

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();
	const float scaleX = sx * scale;
	const float scaleY = sy * scale;
	float x = -1.0f;
	float y = -1.0f;

	std::vector<Text::CharacterRect> characterRects = text.getCharacterRects();
	for (unsigned int i = 0; i < characterRects.size(); i++)
	{
		Text::CharacterRect rect = characterRects[i];
		rect.rect.tl.x = rect.rect.tl.x*scaleX + x;
		rect.rect.tl.y = rect.rect.tl.y*scaleY + y;
		rect.rect.tr.x = rect.rect.tr.x*scaleX + x;
		rect.rect.tr.y = rect.rect.tr.y*scaleY + y;
		rect.rect.bl.x = rect.rect.bl.x*scaleX + x;
		rect.rect.bl.y = rect.rect.bl.y*scaleY + y;
		rect.rect.br.x = rect.rect.br.x*scaleX + x;
		rect.rect.br.y = rect.rect.br.y*scaleY + y;

		this->vb->updateData(&(rect.rect.tl.x), sizeof(Text::CharacterRect::Rect));

		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, rect.textureID);

		this->va->bind();
		this->vb->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void GUIRenderer::draw(Text & text, float x, float y)
{
	this->shader->bind();
	this->shader->setTexture2D("tex", 0, *text.getTexture());
	this->shader->setUniform2f("pos", x+1.0f, y+1.0f);
	this->vaQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->shader->unbind();
}

void GUIRenderer::draw(Text & text, float x, float y, float scale)
{
	Font* font = text.getFont();
	Display& display = Display::get();

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();
	const float scaleX = sx * scale;
	const float scaleY = sy * scale;

	std::vector<Text::CharacterRect> characterRects = text.getCharacterRects();
	for (unsigned int i = 0; i < characterRects.size(); i++)
	{
		Text::CharacterRect rect = characterRects[i];
		rect.rect.tl.x = rect.rect.tl.x*scaleX + x;
		rect.rect.tl.y = rect.rect.tl.y*scaleY + y;
		rect.rect.tr.x = rect.rect.tr.x*scaleX + x;
		rect.rect.tr.y = rect.rect.tr.y*scaleY + y;
		rect.rect.bl.x = rect.rect.bl.x*scaleX + x;
		rect.rect.bl.y = rect.rect.bl.y*scaleY + y;
		rect.rect.br.x = rect.rect.br.x*scaleX + x;
		rect.rect.br.y = rect.rect.br.y*scaleY + y;

		this->vb->updateData(&(rect.rect.tl.x), sizeof(Text::CharacterRect::Rect));
		
		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, rect.textureID);
		
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GUIRenderer::initTextRendering()
{
	// Vao for each character
	this->va = new VertexArray();
	this->vb = new VertexBuffer(NULL, sizeof(float) * 4*4, GL_DYNAMIC_DRAW);
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
	this->fb.attachTexture(display.getWidth(), display.getHeight(), AttachmentType::DEPTH);
}
