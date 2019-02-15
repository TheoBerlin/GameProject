#include "GUIRenderer.h"

#include "../GUI/FontManager.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Utils/Logger.h"

#include "../GUI/GUIManager.h"

#include "../Config.h"
#include "../Imgui/imgui.h"

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

void GUIRenderer::draw(GUIManager & guiManger)
{
	prepareTextRendering();
	std::vector<Panel*>& panelList = guiManger.getPanelList();
	for (Panel* panel : panelList)
	{
		draw(panel);
		drawBaked(panel);
	}
}

void GUIRenderer::bakeText(Text & text, float scale)
{
	Display& display = Display::get();

	if (scale > 0.0f)
		text.setScale(scale);

	fb.updateDimensions(0, (GLuint)text.getWidth(), (GLuint)text.getHeight());
	fb.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	drawToBake(text);
	fb.unbind();
	text.setBakedTexture(*fb.getColorTexture(0));
	text.getBakedTexture()->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glViewport(0, 0, display.getWidth(), display.getHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawBaked(Text & text, float x, float y, float sx, float sy)
{
	float scaleX = sx * text.getScale()*0.5f;
	float scaleY = sy * text.getScale()*0.5f;

	this->textShader->bind();
	this->textShader->setTexture2D("tex", 0, *text.getBakedTexture());
	this->textShader->setUniform2f("pos", x + text.getWidth()*scaleX, y + text.getHeight()*scaleY);
	this->textShader->setUniform2f("scale", text.getWidth()*scaleX, text.getHeight()*scaleY);
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->textShader->unbind();
}

void GUIRenderer::draw(Text & text, float x, float y, float scale)
{
	Font* font = text.getFont();
	Display& display = Display::get();

	float currentScale = scale <= 0.0f ? text.getScale() : scale;

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
	LOG_WARNING("BAKED PANEL");
	// Bake sub-panels and their text.
	std::vector<Panel*>& children = panel->getChildren();
	for (Panel* child : children)
		bakePanel(child);

	Display& display = Display::get();
	GLuint windowWidth = (GLuint)display.getWidth();
	GLuint windowHeight = (GLuint)display.getHeight();

	GLuint fbW = (GLuint)(panel->getSize().x*windowWidth*0.5f);
	GLuint fbH = (GLuint)(panel->getSize().y*windowHeight*0.5f);

	for (auto& element : panel->getTextList())
		bakeText(*element.first, -1.0f);

	fb.updateDimensions(0, fbW, fbH);
	fb.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render current panel
	this->panelShader->bind();
	this->panelShader->setUniform2f("pos", 0.0f, 0.0f);
	this->panelShader->setUniform2f("size", 2.0f, 2.0f);
	this->panelShader->setUniform4f("color", panel->getColor().x, panel->getColor().y, panel->getColor().z, panel->getColor().w);
	Texture* texture = panel->getBackgroundTexture();
	if (texture == nullptr)
		texture = this->whiteOnePixTexture;
	this->panelShader->setTexture2D("tex", 0, *texture);

	this->vaQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw text.
	for (auto& element : panel->getTextList())
	{
		float x = element.second.x;
		float y = element.second.y;
		element.first->getBakedTexture()->bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		drawBaked(*element.first, x, y, 1.0f/(float)fbW, 1.0f/(float)fbH);
	}

	// Draw sub-panels baked texture.
	for (Panel* child : children)
		drawBaked(child, panel->getPosition());

	fb.unbind();
	panel->setBakedTexture(*fb.getColorTexture(0));
	panel->getBakedTexture()->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void GUIRenderer::drawBaked(Panel * panel, const glm::vec2& relativePos)
{
	Display& display = Display::get();

	float windowWidth = display.getWidth();
	float windowHeight = display.getHeight();

	float x = relativePos.x + panel->getPosition().x;
	float y = relativePos.y + panel->getPosition().y;

	this->textShader->bind();
	this->textShader->setTexture2D("tex", 0, *panel->getBakedTexture());
	this->textShader->setUniform2f("pos", x, y);
	this->textShader->setUniform2f("scale", panel->getSize().x*0.5f, panel->getSize().y*0.5f);
	this->vaFullQuad->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	this->textShader->unbind();


	#ifdef IMGUI
		auto renderTexture = [](Texture* texture) {
			Display& display = Display::get();
			ImTextureID texID = (ImTextureID)texture->getID();
			float ratio = (float)texture->getWidth() / (float)texture->getHeight();
			ImGui::Image(texID, ImVec2(50, 50 / ratio), ImVec2(0, 1), ImVec2(1, 0));
			if (false)
				ImGui::SameLine();
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image(texID, ImVec2(display.getWidth()*0.5f, display.getWidth()*0.5f/ratio), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			ImGui::Text("Size: %d, %d", texture->getWidth(), texture->getHeight());
		};
		//Create new frame for ImGui
		ImGui::Begin("GUI textures");
		renderTexture(panel->getTextList()[0].first->getBakedTexture());
		renderTexture(panel->getBakedTexture());
		ImGui::End();
	#endif /* IMGUI */
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

	Display& display = Display::get();
	float sx = display.getPixelXScale();
	float sy = display.getPixelYScale();

	for (auto& element : panel->getTextList())
	{
		float x = panel->getPosition().x + element.second.x;
		float y = panel->getPosition().y + element.second.y;
		drawBaked(*element.first, x, y, sx, sy);
	}
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
	this->fb.getColorTexture(0)->bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::vector<unsigned char> data = { 0xFF, 0xFF, 0xFF, 0xFF };
	this->whiteOnePixTexture = new Texture(data.data(), 1, 1);
}
