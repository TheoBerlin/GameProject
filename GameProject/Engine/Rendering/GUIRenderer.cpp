#include "GUIRenderer.h"

#include "../GUI/FontManager.h"

GUIRenderer::GUIRenderer()
{
	FontManager::addFont("times", "./Game/assets/fonts/times/times.ttf", 12);
}

GUIRenderer::~GUIRenderer()
{
}

void GUIRenderer::draw()
{
}
