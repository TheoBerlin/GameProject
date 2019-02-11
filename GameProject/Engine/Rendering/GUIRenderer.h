#pragma once

#include "GLAbstraction/VertexArray.h"
#include "../GUI/Text.h"

class GUIRenderer
{
public:
	GUIRenderer();
	virtual ~GUIRenderer();

	void draw(Text & text, float x, float y, float scale, const std::string& fontName);
	void draw(const std::string& str, float x, float y, float scale, const std::string& fontName);

private:
	void initTextRendering();
	VertexArray* va;
	VertexBuffer* vb;
};
