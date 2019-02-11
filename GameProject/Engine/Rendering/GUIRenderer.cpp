#include "GUIRenderer.h"

#include "../GUI/FontManager.h"

GUIRenderer::GUIRenderer()
{
	initTextRendering();
}

GUIRenderer::~GUIRenderer()
{
	delete this->va;
}

void GUIRenderer::draw(Text & text, float x, float y, float scale, const std::string& fontName)
{
	Font* font = FontManager::getFont(fontName);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Display& display = Display::get();
	std::vector<Text::CharacterRect> characterRects = text.getCharacterRects();
	const float scaleX = 2.f*display.getPixelXScale() * scale;
	const float scaleY = 2.f*display.getPixelYScale() * scale;
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

		this->vb->updateData(&rect.rect.tl[0], sizeof(Text::CharacterRect::Rect));
		
		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, rect.textureID);
		
		this->va->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void GUIRenderer::draw(const std::string & str, float x, float y, float scale, const std::string & fontName)
{
	Font* font = FontManager::getFont(fontName);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Display& display = Display::get();

	const char* p;
	for (p = str.c_str(); *p; p++)
	{
		Font::Character character = font->getCharacter(*p);

		float x2 = x + character.bearingX * display.getPixelXScale() * scale * 2.f;
		float y2 = y + character.bearingY * display.getPixelYScale() * scale * 2.f;
		float w = character.width * display.getPixelXScale() * scale * 2.f;
		float h = character.height * display.getPixelYScale() * scale * 2.f;

		GLfloat rect[4][4] = {
			{ x2,     y2    , 0, 0 },
			{ x2 + w, y2    , 1, 0 },
			{ x2,     y2 + h, 0, 1 },
			{ x2 + w, y2 + h, 1, 1 },
		};
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		this->vb->updateData(&(rect[0][0]), sizeof(rect));

		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, color);
		font->getShader()->setTexture2D("tex", 0, character.textureID);

		this->va->bind();
		this->vb->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (character.advance >> 6) * display.getPixelXScale() * scale * 2.f;
	}
}

void GUIRenderer::initTextRendering()
{
	this->va = new VertexArray();
	this->vb = new VertexBuffer(NULL, sizeof(float) * 16, GL_DYNAMIC_DRAW);
	AttributeLayout layout;
	layout.push(4); // vec4 Position and uv.
	this->va->addBuffer(vb, layout);
}
