#include "GUIRenderer.h"

#include "../GUI/FontManager.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Utils/Logger.h"

GUIRenderer::GUIRenderer()
{
	this->textShader = new Shader("./Engine/Rendering/Shaders/TexShader.vert", "./Engine/Rendering/Shaders/TexShader.frag");
	this->panelShader = new Shader("./Engine/Rendering/Shaders/PanelShader.vert", "./Engine/Rendering/Shaders/PanelShader.frag");
	initTextRendering();
}

GUIRenderer::~GUIRenderer()
{
	delete this->vaFullQuad;
	delete this->vaQuad;
	delete this->textShader;
	delete this->panelShader;
	delete this->whiteOnePixTexture;
}

void GUIRenderer::bakeText(Text & text, float scale)
{
	Display& display = Display::get();

	if (scale > 0.0f)
		text.setScale(scale);

	fb.updateDimensions(0, text.getWidth(), text.getHeight());
	fb.bind();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	drawToBake(text);
	fb.unbind();
	text.setBakedTexture(fb.getColorTexture(0));

	glViewport(0, 0, display.getWidth(), display.getHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawBaked(Text & text, float x, float y)
{
	Display& display = Display::get();

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();

	float scaleX = sx * text.getScale();
	float scaleY = sy * text.getScale();

	this->textShader->bind();
	this->textShader->setTexture2D("tex", 0, *text.getBakedTexture());
	this->textShader->setUniform2f("pos", x + text.getWidth()*scaleX*0.5f, y + text.getHeight()*scaleY*0.5f);
	this->textShader->setUniform2f("scale", text.getWidth()*scaleX*0.5f, text.getHeight()*scaleY*0.5f);
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->textShader->unbind();
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
		
		this->vaQuad->bind();
		this->vbQuad->bind();
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
		this->vbQuad->updateData(rect, sizeof(rect));
		
		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &(color.x));
		font->getShader()->setTexture2D("tex", 0, character.textureID);

		this->vaQuad->bind();
		this->vbQuad->bind();
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

void GUIRenderer::bakePanel(Panel * panel)
{
	std::vector<std::pair<Text*, glm::vec2>>& textList = panel->getTextList();


	fb.updateDimensions(0, panel->getSize().x, panel->getSize().y);
	fb.bind();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// TODO:
	// Draw sub-panel and their text.
	// Draw panel.
	// Draw text.

	fb.unbind();
	panel->setBakedTexture(fb.getColorTexture(0));

	Display& display = Display::get();
	glViewport(0, 0, display.getWidth(), display.getHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawBaked(Panel * panel)
{
	Display& display = Display::get();

	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();

	float x = panel->getPosition().x;
	float y = panel->getPosition().y;

	this->textShader->bind();
	this->textShader->setTexture2D("tex", 0, *panel->getBakedTexture());
	this->textShader->setUniform2f("pos", x + panel->getSize().x*sx*0.5f, y + panel->getSize().y*sy*0.5f);
	this->textShader->setUniform2f("scale", panel->getSize().x*sx*0.5f, panel->getSize().y*sy*0.5f);
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->textShader->unbind();
}

void GUIRenderer::draw(Panel * panel)
{
	this->panelShader->bind();
	this->panelShader->setUniform2f("pos", panel->getPosition().x, panel->getPosition().y);
	this->panelShader->setUniform2f("size", panel->getSize().x, panel->getSize().y);
	this->panelShader->setUniform4f("color", panel->getColor().x, panel->getColor().y, panel->getColor().z, panel->getColor().w);
	Texture* texture = panel->getBackgroundTexture();
	if (texture == nullptr)
		texture = this->whiteOnePixTexture;
	this->panelShader->setTexture2D("tex", 0, *texture);

	this->vaQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GUIRenderer::drawToBake(Text & text)
{
	Font* font = text.getFont();

	Display& display = Display::get();

	float sx = 1.0f / (float)(text.getWidth());
	float sy = 1.0f / (float)(text.getHeight());
	const float scaleX = sx * 2.0f;
	const float scaleY = sy * 2.0f;
	float x = -1.0f;
	float y = -1.0f + (text.getHeight() - text.getBearingY())*scaleY;

	std::vector<Text::CharacterDrawData> charactersData = text.getCharactersDrawData();
	for (unsigned int i = 0; i < charactersData.size(); i++)
	{
		Text::CharacterDrawData character = charactersData[i];

		font->getShader()->bind();
		font->getShader()->setUniform4fv("color", 1, &text.getColor()[0]);
		font->getShader()->setTexture2D("tex", 0, character.textureID);
		font->getShader()->setUniform2f("scale", character.scale.x * scaleX, character.scale.y * scaleY);
		font->getShader()->setUniform2f("pos", character.pos.x * scaleX + x, character.pos.y * scaleY + y);

		this->vaQuad->bind();
		this->vbQuad->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

void GUIRenderer::initTextRendering()
{
	// Vao for each character
	this->vaQuad = new VertexArray();
	GLfloat quad[4][4] = {
			{ -0.5f, +0.5f, 0, 0 }, // TL
			{ -0.5f, -0.5f, 0, 1 }, // BL
			{ +0.5f, +0.5f, 1, 0 }, // TR
			{ +0.5f, -0.5f, 1, 1 }, // BR
	};
	this->vbQuad = new VertexBuffer(quad, sizeof(quad), GL_STATIC_DRAW);
	AttributeLayout layout;
	layout.push(4); // [vec4] Position and uv.
	this->vaQuad->addBuffer(vbQuad, layout);
	
	// Vao for baked texture.
	this->vaFullQuad = new VertexArray();
	GLfloat fullQuad[4][4] = {
			{ -1.0f, +1.0f, 0, 0 }, // TL
			{ -1.0f, -1.0f, 0, 1 }, // BL
			{ +1.0f, +1.0f, 1, 0 }, // TR
			{ +1.0f, -1.0f, 1, 1 }, // BR
	};
	this->vbFullQuad = new VertexBuffer(fullQuad, sizeof(fullQuad), GL_STATIC_DRAW);
	this->vaFullQuad->addBuffer(vbFullQuad, layout);
	
	Display& display = Display::get();
	this->fb.attachTexture(display.getWidth(), display.getHeight(), AttachmentType::COLOR);

	std::vector<unsigned char> data = { 0xFF, 0xFF, 0xFF, 0xFF };
	this->whiteOnePixTexture = new Texture(data.data(), 1, 1);
}
