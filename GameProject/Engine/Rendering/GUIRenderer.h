#pragma once

#include "GLAbstraction/VertexArray.h"
#include "GLAbstraction/Framebuffer.h"
#include "../GUI/Text.h"
#include "../GUI/Panel.h"

class GUIManager;
class GUIRenderer
{
public:
	GUIRenderer();
	virtual ~GUIRenderer();

	/*
	Draw all elements in guiManager.
	*/
	void draw(GUIManager& guiManger);

	/*
	Bake text into texture.
	*/
	void bakeText(Text& text, float scale);

	/*
	Draw the text with its internal baked texture.
	*/
	void drawBaked(Text & text, float x, float y, float sx, float sy);

	/*
	Draw each character. One draw call for each character.
	*/
	void draw(Text & text, float x, float y, float scale = -1.0f);

	/*
	Cunstruct all character bitmap and draw each character. One draw call for each character.
	*/
	void draw(const std::string& str, float x, float y, const glm::vec4& color, const std::string& fontName, float scale);

	/*
	Enable and set the blend function.
	*/
	void prepareTextRendering();

	void bakePanel(Panel* panel);
	void drawBaked(Panel* panel, const glm::vec2& relativePos = { 0.0f, 0.0f });
	void draw(Panel* panel);

private:
	/*
	Draw the text to the framebuffer.
	*/
	void drawToBake(Text& text);

	void initTextRendering();

	VertexArray* vaFullQuad;
	VertexBuffer* vbFullQuad;

	Framebuffer fb;
	VertexArray* vaQuad;
	VertexBuffer* vbQuad;
	Shader* textShader;

	Texture* whiteOnePixTexture;
	Shader* panelShader;
};
