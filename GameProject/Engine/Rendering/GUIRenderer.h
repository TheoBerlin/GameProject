#pragma once

#include "GLAbstraction/VertexArray.h"
#include "GLAbstraction/Framebuffer.h"
#include "../GUI/Text.h"
#include "../GUI/Panel.h"
#include "../Events/EventBus.h"

class GUI;
class GUIRenderer
{
public:
	GUIRenderer();
	virtual ~GUIRenderer();

	/*
	Draw all elements in gui.
	*/
	void draw(GUI& gui);

	/*
	Bake text into texture.
	*/
	void bakeText(Text& text);

	/*
	Draw the text with its internal baked texture.
	*/
	void drawBaked(Text & text, float x, float y);

	/*
	Enable and set the blend function.
	*/
	void prepareTextRendering();

	void bakePanel(Panel* panel);
	void drawBaked(Panel* panel);

private:
	/*
	Draw the text to the framebuffer.
	*/
	void drawToBake(Text& text);

	void initTextRendering();

	void resizeCallback(WindowResizeEvent* evnt);
	void hideKeyCallback(KeyEvent* evnt);

	bool hidden = false;

	glm::mat4 orthoText;
	glm::mat4 orthoDisplay;

	VertexArray* vaFullQuad;
	VertexBuffer* vbFullQuad;

	Framebuffer fb;
	VertexArray* vaQuad;
	VertexBuffer* vbQuad;
	Shader* textShader;

	Texture* whiteOnePixTexture;
	Shader* panelShader;
};
