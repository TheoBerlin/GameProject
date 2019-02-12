#pragma once

#include "GLAbstraction/VertexArray.h"
#include "GLAbstraction/Framebuffer.h"
#include "../GUI/Text.h"

class GUIRenderer
{
public:
	GUIRenderer();
	virtual ~GUIRenderer();

	/*
	Bake text into texture.
	*/
	void bakeText(Text& text, float scale);

	void drawToBake(Text& text, float scale);

	/*
	Draw the text with its internal baked texture.
	*/
	void draw(Text & text, float x, float y);

	/*
	Draw each character. One draw call for each character.
	*/
	void draw(Text & text, float x, float y, float scale);

	/*
	Cunstruct all character bitmap and draw each character. One draw call for each character.
	*/
	void draw(const std::string& str, float x, float y, const glm::vec4& color, const std::string& fontName, float scale);

	void prepareTextRendering();

private:
	void initTextRendering();
	VertexArray* va;
	VertexBuffer* vb;

	Framebuffer fb;
	VertexArray* vaQuad;
	VertexBuffer* vbQuad;
	Shader* shader;
};
